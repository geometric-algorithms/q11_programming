#include "structures.h"

int main()
{
	TrapezoidMap map;
	std::vector<Segment> segments;
	int N;
    cin >> N;
    for (int i = 0; i < N; ++i)
    {
        float x1, y1, x2, y2;
        std::cin >> x1 >> y1 >> x2 >> y2;
        segments.emplace_back(Point(x1, y1), Point(x2, y2));
    }
	float xq, yq;
    cin >> xq >> yq;
    Point queryPoint(xq, yq);

	map.buildMap(segments);

	const Trapezoid* tr = map.localize(queryPoint);

	ofstream out("data.txt");

	for (const auto& seg : segments)
    {
        out << "SEG " << seg.ptLeft.x << " " << seg.ptLeft.y << " "<< seg.ptRight.x << " " << seg.ptRight.y << "\n";
    }

    out << "TRAP_TOP " << tr->top->ptLeft.x << " " << tr->top->ptLeft.y << " "<< tr->top->ptRight.x << " " << tr->top->ptRight.y << "\n";
    out << "TRAP_BOT " << tr->bot->ptLeft.x << " " << tr->bot->ptLeft.y << " "<< tr->bot->ptRight.x << " " << tr->bot->ptRight.y << "\n";
    out << "TRAP_LEFT " << tr->left.x << " " << tr->left.y << "\n";
    out << "TRAP_RIGHT " << tr->right.x << " " << tr->right.y << "\n";

    out << "QUERY " << queryPoint.x << " " << queryPoint.y << "\n";

    out.close();

    return 0;
}
