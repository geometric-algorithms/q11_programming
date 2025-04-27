#include "structures.h"

/**
 * TrapezoidMap class
 * Contains methods to build a trapezoid map from a set of segments
 * Methods to add segments, query the map, and localize points
 * The map is built using a binary tree structure
 * Each trapezoid is represented by a trapezoid structure
 * The trapezoid structure contains pointers to its top, bottom, left, and right segments
 */
GraphNode* TrapezoidMap::mapQuery(Point pTarget,Point pExtra)
{
	assert(_rootNode);
	GraphNode* curNode = _rootNode;
	while (!curNode->getTrapezoid())
	{
		curNode = curNode->nextNode(pTarget,pExtra);
	}
	return curNode;
}

/**
 * Localize method
 * Finds the trapezoid corresponding to a given point
 * @pt: Point to be localized
 * This function first queries the map to find the trapezoid node corresponding to the point
 * It then returns the trapezoid associated with that node
 */
const Trapezoid* TrapezoidMap::localize(Point pt)
{
	return this->mapQuery(pt,pt)->getTrapezoid();
}

/**
 * BuildMap method
 * Constructs the trapezoid map from a set of segments
 * @segments: Vector of segments to be added to the map
 * This function initializes the bounding box and creates the root node
 * It then adds each segment to the map using the addSegment method
 */
void TrapezoidMap::buildMap(std::vector<Segment>& segments)
{
	if (segments.size() < 2) return;

	// random shuffle the input
	random_device rd;
	default_random_engine rng(rd());
	shuffle(segments.begin(), segments.end(), rng);

	_segments = segments;

	//bounding box Initialize with bounding box
	float minX = -100;
	float minY = -100;
	float maxX = 100;
	float maxY = 100;

	Trapezoid* tp = new Trapezoid;
	_segments.push_back(Segment(Point(minX, maxY), Point(maxX, maxY)));
	tp->top = &_segments.back();
	_segments.push_back(Segment(Point(minX, minY), Point(maxX, minY)));
	tp->bot = &_segments.back();
	tp->left = Point(minX, maxY);
	tp->right = Point(maxX, maxY);
	
	_rootNode = new TerminalNode(tp);
	
	for (auto &segment : segments)
	{
		// add segments 
		this->addSegment(&segment);
	}
}


/**
 * GetNextIntersecting method
 * Finds the next trapezoid intersecting with a given segment
 * @segment: Segment to be checked
 * @tr: Current trapezoid
 * This function checks the right top and bottom trapezoids of the current trapezoid
 * It returns the next trapezoid that intersects with the segment
 */
Trapezoid* getNextIntersecting(Segment* segment, Trapezoid* tr)
{
	assert(tr->trRightTop || tr->trRightBot);
	if(tr->trRightTop==nullptr) return tr->trRightBot;
	if(tr->trRightBot==nullptr) return tr->trRightTop;
	Trapezoid* trNext;
	Point pt = segment->ptWithX(tr->trRightTop->left.x);
	if (tr->trRightTop->bot->isAbove(pt, segment->ptLeft))
	{
		trNext = tr->trRightTop;
	}
	else
	{ 
		trNext = tr->trRightBot;
	}
	return trNext;
}

/**
 * AddSegment method
 * Adds a segment to the trapezoid map
 * @segment: Segment to be added
 * This function first queries the map to find the trapezoid nodes corresponding to the segment
 * It then checks if the segment lies completely inside one trapezium or in multiple trapeziums
 * It calls the appropriate case method (Case1 or Case2) to handle the addition of the segment
 */
void TrapezoidMap::addSegment(Segment* segment)
{
	GraphNode* node1 = this->mapQuery(segment->ptLeft,segment->ptRight);
	GraphNode* node2 = this->mapQuery(segment->ptRight,segment->ptLeft);
	Trapezoid* tp1 = node1->getTrapezoid();
	Trapezoid* tp2 = node2->getTrapezoid();

	if (tp1 == tp2)
	{
		// Case1 segment lies completely inside one trapezium
		this->Case1(node1, segment);
	}
	else
	{
		// Case 2 segment lies in multiple trapeziums
		this->Case2(node1, node2, segment);
	}
}

/**
 * Case1 method
 * Handles the case where a segment lies completely inside one trapezium
 * @tpNode: Trapezoid node corresponding to the segment
 * @segment: Segment to be added
 * This function creates new trapezoids by copying the original trapezoid and modifying its properties
 * It updates the neighbours of the trapezoids and the graph structure accordingly
 */
void TrapezoidMap::Case1(GraphNode* tpNode, Segment* segment)
{
	// Copy constructor to create new trapezoid and change right point (A)
	Trapezoid* trLeft = new Trapezoid(*tpNode->getTrapezoid());
	trLeft->right = segment->ptLeft;

	// Copy constructor to create new trapezoid and change left point (D)
	Trapezoid* trRight = new Trapezoid(*tpNode->getTrapezoid());
	trRight->left = segment->ptRight;

	// Copy constructor to create new trapezoid and change top segment (B)
	Trapezoid* trBot = new Trapezoid(*tpNode->getTrapezoid());
	trBot->top = segment;
	trBot->left = segment->ptLeft;
	trBot->right = segment->ptRight;

	// Copy constructor to create new trapezoid and change bottom segment (C)
	Trapezoid* trTop = new Trapezoid(*tpNode->getTrapezoid());
	trTop->bot = segment;
	trTop->left = segment->ptLeft;
	trTop->right = segment->ptRight;
	
	///updating neighbours for the trapeziums
	trLeft->trRightTop = trTop;
	trLeft->trRightBot = trBot;
	trRight->trLeftTop = trTop;
	trRight->trLeftBot = trBot;

	trTop->setOneRight(trRight);
	trTop->setOneLeft(trLeft);
	trBot->setOneRight(trRight);
	trBot->setOneLeft(trLeft);

	tpNode->getTrapezoid()->changeLeftWith(trLeft);
	tpNode->getTrapezoid()->changeRightWith(trRight);
	
	//updating graph
	GraphNode* newRoot = new XNode(segment->ptLeft.x);
	GraphNode* x2 = new XNode(segment->ptRight.x);
	GraphNode* y1 = new YNode(segment);
	newRoot->attachLeft(new TerminalNode(trLeft));
	newRoot->attachRight(x2);

	x2->attachRight(new TerminalNode(trRight));
	x2->attachLeft(y1);

	y1->attachLeft(new TerminalNode(trTop));
	y1->attachRight(new TerminalNode(trBot));
	
	if (tpNode == _rootNode)
	{
		_rootNode = newRoot;
	}
	else
	{
		tpNode->replaceWith(newRoot);
	}
}

/**
 * Case2 method
 * Handles the case where a segment lies in multiple trapeziums
 * @pLeft: Left trapezoid node corresponding to the segment
 * @pRight: Right trapezoid node corresponding to the segment
 * @segment: Segment to be added
 * This function creates new trapezoids by copying the original trapezoids and modifying their properties
 * It updates the neighbours of the trapezoids and the graph structure accordingly
 */
void TrapezoidMap::Case2(GraphNode* pLeft, GraphNode* pRight, Segment* segment)
{
	Trapezoid* trBegin = pLeft->getTrapezoid();
	Trapezoid* trEnd = pRight->getTrapezoid();

	//leftmost one left of the segment
	Trapezoid* trLeftmost = new Trapezoid(*trBegin);
	trLeftmost->right = segment->ptLeft;

	// Top half of orginal start trapezium created 
	Trapezoid* trTopHalf = new Trapezoid(*trBegin);
	trTopHalf->left = segment->ptLeft;
	trTopHalf->bot = segment;

	// Bottom half of orginal start trapezium created 
	Trapezoid* trBotHalf = new Trapezoid(*trBegin);
	trBotHalf->left = segment->ptLeft;
	trBotHalf->top = segment;

	// update neighbours
	trLeftmost->trRightTop = trTopHalf;
	trLeftmost->trRightBot = trBotHalf;
	trBegin->changeLeftWith(trLeftmost);
	trTopHalf->setOneLeft(trLeftmost);
	trBotHalf->setOneLeft(trLeftmost);
	
	
	//updating graph
	GraphNode* terminalTop = new TerminalNode(trTopHalf);
	GraphNode* terminalBot = new TerminalNode(trBotHalf);
	GraphNode* newLeft = new XNode(segment->ptLeft.x);
	GraphNode* newSplit = new YNode(segment);
	newLeft->attachLeft(new TerminalNode(trLeftmost));
	newLeft->attachRight(newSplit);
	newSplit->attachLeft(terminalTop);
	newSplit->attachRight(terminalBot);
	trBegin->graphNode->replaceWith(newLeft);
	Trapezoid* trPrev = trBegin;Trapezoid* trCurrent = getNextIntersecting(segment, trBegin);

	//middle intersecting
	while(true)
	{
		bool renewTop = false;

		if (trPrev->trRightBot && trPrev->trRightTop) renewTop = (trPrev->trRightBot == trCurrent);
		else renewTop = (trCurrent->trLeftBot == trPrev);

		if (renewTop)
		{
			trTopHalf->right = trCurrent->left;
			Trapezoid* oldMergeTop = trTopHalf;
			trTopHalf = new Trapezoid(*trCurrent);
			terminalTop = new TerminalNode(trTopHalf);
			trTopHalf->bot = segment;

			if (trCurrent->trLeftBot && trCurrent->trLeftTop)
			{
				oldMergeTop->setOneRight(trTopHalf);
				trTopHalf->trLeftBot = oldMergeTop;
				trTopHalf->trLeftTop = trCurrent->trLeftTop;
				trCurrent->trLeftTop->setOneRight(trTopHalf);
				assert(trTopHalf->trLeftTop);
			}
			else
			{
				oldMergeTop->trRightBot = trTopHalf;
				oldMergeTop->trRightTop = trPrev->trRightTop;
				trPrev->trRightTop->setOneLeft(oldMergeTop);
				trTopHalf->setOneLeft(oldMergeTop);
			}
		}
		else
		{
			assert(trPrev->trRightTop == trCurrent);
			trBotHalf->right = trCurrent->left;
			Trapezoid* oldMergeBot = trBotHalf;
			trBotHalf = new Trapezoid(*trCurrent);
			terminalBot = new TerminalNode(trBotHalf);
			trBotHalf->top = segment;

			if (trCurrent->trLeftBot && trCurrent->trLeftTop)
			{
				oldMergeBot->setOneRight(trBotHalf);
				trBotHalf->trLeftTop = oldMergeBot;
				trBotHalf->trLeftBot = trCurrent->trLeftBot;
				trCurrent->trLeftBot->setOneRight(trBotHalf);
				assert(trBotHalf->trLeftBot);
			}
			else
			{
				oldMergeBot->trRightTop = trBotHalf;
				oldMergeBot->trRightBot = trPrev->trRightBot;
				trPrev->trRightBot->setOneLeft(oldMergeBot);
				trBotHalf->setOneLeft(oldMergeBot);
			}
		}

		if (trCurrent == trEnd) break;

		
		// update graph
		newSplit = new YNode(segment);
		newSplit->attachLeft(terminalTop);
		newSplit->attachRight(terminalBot);
		trCurrent->graphNode->replaceWith(newSplit);

		trPrev = trCurrent;
		trCurrent = getNextIntersecting(segment, trCurrent);
	}

	trTopHalf->right = segment->ptRight;
	trBotHalf->right = segment->ptRight;

	//rightmost one (final one)
	Trapezoid* trRightmost = new Trapezoid(*trEnd);
	trRightmost->left = segment->ptRight;

	//update neighbors
	trTopHalf->setOneRight(trRightmost);
	trBotHalf->setOneRight(trRightmost);
	trRightmost->trLeftTop = trTopHalf;
	trRightmost->trLeftBot = trBotHalf;
	trEnd->changeRightWith(trRightmost);
	
	// update the graph
	GraphNode* newRight = new XNode(segment->ptRight.x);
	newSplit = new YNode(segment);
	newRight->attachRight(new TerminalNode(trRightmost));
	newRight->attachLeft(newSplit);
	newSplit->attachLeft(terminalTop);
	newSplit->attachRight(terminalBot);
	trEnd->graphNode->replaceWith(newRight);
	
}
