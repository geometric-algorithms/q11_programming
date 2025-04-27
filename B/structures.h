#include <bits/stdc++.h>
using namespace std;

/**
 * Point structure representing a point in 2D space.
 * Contains x and y coordinates.
 * Provides constructors for initialization.
 */
struct Point
{	float x,y;
	Point(float x = 0.0f, float y = 0.0f): x(x), y(y) {}
};

/**
 * Segment structure
 * Contains two points (ptLeft, ptRight) and methods to check if a point is above the segment
 * and to get x/y coordinates based on y/x coordinates
 * isAbove() checks if a point is above the segment
 * ptWithX() returns the point in the segment with x-coordinate x
 * detHelper() calculates the determinant for checking the position of a point relative to the segment
 */
struct Segment
{
	Point ptLeft;
	Point ptRight;
	Segment(Point pt1, Point pt2): ptLeft(pt1), ptRight(pt2) 
	{
		if  (ptLeft.x >  ptRight.x || (ptLeft.x == ptRight.x && ptLeft.y > ptRight.y)) swap(ptLeft, ptRight);
	}
	float detHelper(Point p)
	{
		return ((ptRight.x - ptLeft.x) * (p.y - ptLeft.y) - 
				(ptRight.y - ptLeft.y) * (p.x - ptLeft.x));
	}
	bool isAbove(Point pTarget, Point pGuide)
	{
		// find if target point is above break ties (numerical zeros) with pGuide
		float EPS = 0.1;
		float det = this->detHelper(pTarget);
		return (fabsf(det) > EPS) ? det > 0 : this->detHelper(pGuide) > 0;
	}
	Point ptWithX(float x)
	{
		// find point in the segment with x-cord x
		float y = ptLeft.y + (ptRight.y - ptLeft.y) / (ptRight.x - ptLeft.x) * (x - ptLeft.x);
		return Point(x, y);
	}

	float 	minY() 	{return std::min(ptLeft.y, ptRight.y);}
	float 	maxY() 	{return std::max(ptLeft.y, ptRight.y);}

};


struct GraphNode;

/**
 * Trapezoid structure
 */
struct Trapezoid
{
	// defines the trapezium
	Segment* top;
	Segment* bot;
	Point left;
	Point right;

	// neighbours 
	Trapezoid* trRightBot; // lower right neighbour
	Trapezoid* trRightTop; // upper right neighbour
	Trapezoid* trLeftTop;  // upper left neighbour
	Trapezoid* trLeftBot;  // lower left neighbour

	// corresponding node in DAG
	GraphNode* graphNode;

	Trapezoid(): trRightBot(nullptr), trRightTop(nullptr),
				 trLeftTop(nullptr), trLeftBot(nullptr),
				 graphNode(nullptr){}

	/**
	 * Setters for trapezoid
	 * Set the top and bottom segments of the trapezoid
	 * Set the left and right points of the trapezoid
	 * Set the left and right neighbours of the trapezoid
	 * Set the trapezoid to be a terminal node
	 * Set the trapezoid to be a non-terminal node
	 * Set the trapezoid to be a non-terminal node with one neighbour
	 */
	void setOneRight(Trapezoid* tp) {trRightTop = tp; trRightBot = nullptr;}
	
	void setOneLeft(Trapezoid* tp) {trLeftTop = tp; trLeftBot = nullptr;}

	void changeLeftWith(Trapezoid* tp)
	{
		// updating left neighbours
		if (trLeftTop)
		{
			assert(trLeftTop->trRightTop == this || trLeftTop->trRightBot == this);
			if (trLeftTop->trRightTop == this) trLeftTop->trRightTop = tp;
			else trLeftTop->trRightBot = tp;
		}
		if (trLeftBot)
		{
			assert(trLeftBot->trRightTop == this || trLeftBot->trRightBot == this);
			if (trLeftBot->trRightTop == this) trLeftBot->trRightTop = tp;
			else trLeftBot->trRightBot = tp;
		}
	}

	void changeRightWith(Trapezoid* tp)
	{
		// updating right neighbours
		if (trRightTop)
		{
			assert(trRightTop->trLeftTop == this || trRightTop->trLeftBot == this);
			if (trRightTop->trLeftTop == this) trRightTop->trLeftTop = tp;
			else trRightTop->trLeftBot = tp;
		}
		if (trRightBot)
		{
			assert(trRightBot->trLeftTop == this || trRightBot->trLeftBot == this);
			if (trRightBot->trLeftTop == this) trRightBot->trLeftTop = tp;
			else trRightBot->trLeftBot = tp;
		}
	}
};



class GraphNode 
{
public:
	GraphNode* _left;
	GraphNode* _right;
	//list<GraphNode*> _parents;
	vector<GraphNode*> _parents;
	GraphNode(): _left(nullptr), _right(nullptr) {}
	virtual ~GraphNode() {}
	virtual Trapezoid* 	getTrapezoid() 			{return nullptr;}
	virtual GraphNode* 	nextNode(Point,Point) 	{return nullptr;}
	
	void attachLeft(GraphNode* node) 
	{
		// add this node to the left child
		_left = node;
		node->_parents.push_back(this);
	}
	
	void attachRight(GraphNode* node)
	{
		// add this node to the right child
		_right = node;
		node->_parents.push_back(this);
	}
	
	void replaceWith(GraphNode* node)
	{
		// change urself with node
		assert(!_parents.empty());
		for (auto parent : _parents)
		{
			if (parent->_left == this)
			{
				parent->_left = node;
			}
			else
			{
				assert(parent->_right == this);
				parent->_right = node;
			}
		}
	}
};

class XNode: public GraphNode
{
public:
	float _point;
	XNode(float p): _point(p) {}

	virtual GraphNode* nextNode(Point p,Point)
	{
		return (p.x < _point) ? _left : _right;
	}
};

class YNode: public GraphNode
{
public:
	Segment* _segment;
	YNode(Segment* s): _segment(s) {}

	virtual GraphNode* nextNode(Point pTarget,Point pGuide)
	{
		return _segment->isAbove(pTarget,pGuide) ? _left : _right;
	}
};

class TerminalNode: public GraphNode
{
public:
	Trapezoid* _trapezoid;
	TerminalNode(Trapezoid* tp): _trapezoid(tp) {tp->graphNode = this;}
	virtual ~TerminalNode() 			{delete _trapezoid;}
	virtual Trapezoid* getTrapezoid() 	{return _trapezoid;}
};

class TrapezoidMap
{
public:
	GraphNode* 				_rootNode;
	vector<Segment> 	    _segments;

	TrapezoidMap():_rootNode(nullptr){}
	
	void 		addSegment(Segment* segment); // add segment into T and D

	GraphNode* 	mapQuery(Point pTarget,Point pExtra); // find Trapezoid node coresponding to the point
	const Trapezoid* localize(Point pt); // Find Trapezoid corresponding to the point

	void 		Case1(GraphNode* tpNode, Segment* segment);
	void		Case2(GraphNode* pLeft, GraphNode* pRight, Segment* segment);
	void		buildMap(std::vector<Segment>& segments);

	~TrapezoidMap(){}

};
