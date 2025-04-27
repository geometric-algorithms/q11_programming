#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <string>
#include <set>
#include <map>
#include <utility>
#include <algorithm>
#include <iomanip>
#include <stack>
#include <queue>
#include <chrono>
#include <unordered_set>
#include <unordered_map>
#include <random>
#include <bitset>
#include <sstream>

using namespace std;
vector<double> x_coords;  // Sorted x-coordinates 
double xmin = 100,xmax = -100,ymin = 100,ymax = -100;
ofstream out("data.txt");

/**
 * Point structure representing a point in 2D space.
 * Contains x and y coordinates.
 * Provides constructors for initialization.
 */
struct Point {
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}
};

/**
 * Segment structure
 * Contains two points (p1, p2) and an ID
 * Methods to check if a point is above the segment and to get x/y coordinates
 * based on y/x coordinates
 * isAbove() checks if a point is above the segment
 * getX() returns the x-coordinate of the segment at a given y-coordinate
 * getY() returns the y-coordinate of the segment at a given x-coordinate
 * The segment is represented as a directed line from p1 to p2
 */
struct Segment {
    Point p1, p2;
    int id;
    Segment() 
    {
        p1 = Point();
        p2 = Point();
        id = 0;
    }
    Segment(Point p1, Point p2, int id = 0) 
    {
        this->p1 = p1;
        this->p2 = p2;
        this->id = id;
    }
    bool isAbove(Point p)
    {
        return ((p2.x - p1.x) * (p.y - p1.y) - (p2.y - p1.y) * (p.x - p1.x)) > 0;
    }

    double getX(double y)
    {
        if (p1.y == p2.y) return p1.x;
        return p1.x + (p2.x - p1.x) * (y - p1.y) / (p2.y - p1.y);
    }

    double getY(double x)
    {
        if (p1.x == p2.x) return p1.y;
        return p1.y + (p2.y - p1.y) * (x - p1.x) / (p2.x - p1.x);
    }
};

struct PNode;


/**
 * Node structure
 * Contains a segment, timestamp, and pointers to left and right child nodes
 */
struct Node {
    Segment* segment;
    int timestamp;
    PNode *left, *right;
    Node() 
    {
        left = nullptr;
        right = nullptr;
        segment = new Segment();
        timestamp = 0;
    }
    Node(Segment* seg,int timestamp) 
    {
        segment = seg;
        this->timestamp = timestamp;
        left = nullptr;
        right = nullptr;
    }
    Node(Segment* seg, PNode* left, PNode* right) 
    {
        segment = seg;
        this->timestamp = 0;
        this->left = left;
        this->right = right;
    }
};


/**
 * Persistent node structure
 * Contains a vector of pairs (timestamp, Node*)
 * Each pair represents a version of the tree at a specific timestamp
 */
struct PNode {
    vector<pair<int,Node*> > nodes; 
    PNode() 
    {
        nodes = vector<pair<int,Node*> >();
    }
};

/**
 * Persistent tree structure
 * Contains a root node and size
 * Methods to insert segments, delete segments, and find segments above/below a point
 * createVersion() creates a new version of the tree with given segments
 * findAbove() finds the segment above a point in a specific version
 * findBelow() finds the segment below a point in a specific version
 */
class PersistentTree {
public:
    PNode* root; // Roots of all versions
    int size = 0;

    PersistentTree() 
    { 
        root = nullptr; 
    }
    
    /**
     * Insert segment into the tree
     * Creates a new version of the tree with the segment
     * @seg: Segment to be inserted
     * @timestamp: Timestamp of the version
     * This function traverses the tree to find the correct position for the segment   
     */
    void insert(Segment* seg,int timestamp) 
    {
        PNode* curr = root;
        if(root == nullptr)
        {
            root = new PNode();
            Node* new_node = new Node(seg,timestamp);
            root->nodes.push_back(make_pair(timestamp, new_node));
            return;
        }
        if(root->nodes.back().second==nullptr)
        {
            root->nodes.push_back(make_pair(timestamp, new Node(seg,timestamp)));
            return;
        }
        
        double y_coord = seg->getY(x_coords[timestamp+1]);
        // cout<<y_coord<<endl;
        while(curr != nullptr)
        {
            int size = (curr->nodes).size();
            Node* node = curr->nodes.back().second;
            double ycurr = node->segment->getY(x_coords[timestamp+1]);
            if(timestamp==1)
            {
                // cout<<"[Y] "<<ycurr<<" "<<y_coord<<endl;
            }
            if(y_coord < ycurr)
            {
                if(node->left == nullptr)
                {
                    Node* new_node = new Node(*node);
                    new_node->timestamp = timestamp;
                    PNode* new_pnode = new PNode();
                    new_pnode->nodes.push_back(make_pair(timestamp, new Node(seg,timestamp)));
                    new_node->left = new_pnode;
                    curr->nodes.push_back(make_pair(timestamp, new_node));
                    //cout<<timestamp<<endl;
                    return;
                }
                else
                    curr = node->left;
            }
            else
            {
                if(node->right == nullptr)
                {
                    Node* new_node = new Node(*node);
                    new_node->timestamp = timestamp;
                    PNode* new_pnode = new PNode();
                    new_pnode->nodes.push_back(make_pair(timestamp, new Node(seg,timestamp)));
                    new_node->right = new_pnode;
                    curr->nodes.push_back(make_pair(timestamp, new_node));
                    // cout<<"right add"<<endl;
                    // cout<<timestamp<<endl;
                    return;
                }
                else
                    curr = node->right;
            }    
        }
    }

    /**
     * Delete segment from the tree
     * Creates a new version of the tree without the segment
     * @seg: Segment to be deleted
     * @timestamp: Timestamp of the version
     * This function traverses the tree to find the segment and remove it
     * It handles different cases based on the structure of the tree
     */
    void delSegment(Segment* seg,int timestamp)
    {
        PNode* curr = root;
        PNode* prev = nullptr;
        double y_coord = seg->getY(x_coords[timestamp]);
        while(curr != nullptr && curr->nodes.back().second->segment->id != seg->id)
        {
            prev = curr;
            Node* node = curr->nodes.back().second;
            double ycurr = node->segment->getY(x_coords[timestamp]);
            
            if(y_coord < ycurr)
                curr = node->left;
            else
                curr = node->right;
            
        }
        if(curr == nullptr)
            return;
        Node* node = curr->nodes.back().second;
        if(node->left == nullptr && node->right == nullptr)
        {
            if(prev == nullptr)
            {
                curr->nodes.push_back(make_pair(timestamp, nullptr));
                return;
            }
            Node* new_node = new Node(*(prev->nodes.back().second));;
            new_node->timestamp = timestamp;
            if( new_node->left!=nullptr && new_node->left->nodes.back().second == node )
                new_node->left = nullptr;
            else
                new_node->right = nullptr;
            prev->nodes.push_back(make_pair(timestamp, new_node));
            //cout<<"hi"<<endl;
            return;
        }
        else if(node->left == nullptr)
        {
            if(prev == nullptr)
            {
                curr->nodes.push_back(make_pair(timestamp, curr->nodes.back().second->right->nodes.back().second));
                return;
            }
            Node* new_node = new Node(*(prev->nodes.back().second));
            new_node->timestamp = timestamp;
            new_node->left = node->right;
            prev->nodes.push_back(make_pair(timestamp, new_node));
            return;
        }
        else if(node->right == nullptr)
        {
            if(prev == nullptr)
            {
                curr->nodes.push_back(make_pair(timestamp, curr->nodes.back().second->left->nodes.back().second));
                return;
            }
            Node* new_node = new Node(*(prev->nodes.back().second));
            new_node->timestamp = timestamp;
            new_node->right = node->left;
            prev->nodes.push_back(make_pair(timestamp, new_node));
            return;
        }
        else
        {
            PNode* pred = node->left;
            PNode* prev1 = nullptr;
            while(pred->nodes.back().second->right != nullptr)
            {
                prev1 = pred;
                pred = pred->nodes.back().second->right;
            }
            Node* new_node = new Node(*node);
            new_node->timestamp = timestamp;
            new_node->segment = pred->nodes.back().second->segment;            
            if(prev1 == nullptr)
            {
                new_node->left = nullptr;
            }
            else
            {
                Node* new_node1 = new Node(*(prev1->nodes.back().second));
                new_node1->timestamp = timestamp;
                new_node1->right = pred->nodes.back().second->left;
                prev1->nodes.push_back(make_pair(timestamp, new_node1));
            }
            curr->nodes.push_back(make_pair(timestamp, new_node));
            return;
        }
    }
    
    /**
     * Create a new version of the tree
     * Inserts and deletes segments based on the given vectors
     * @segments: Vector of segments to be inserted
     * @del_seg: Vector of segments to be deleted
     * @ts: Timestamp of the version
     * This function creates a new version of the tree by inserting and deleting segments
     */
    void createVersion(vector<Segment> segments,vector<Segment> del_seg,int ts) {
        for (vector<Segment>::iterator it = del_seg.begin(); it != del_seg.end(); ++it) {
            Segment* seg = new Segment(*it);
            delSegment(seg,ts);
        }
        for (vector<Segment>::iterator it = segments.begin(); it != segments.end(); ++it) {
            Segment* seg = new Segment(*it);
            insert(seg,ts);
        }
    }
    
    /**
     * Find the segment above a point in a specific version
     * @version: Version of the tree
     * @p: Point to be checked
     * This function traverses the tree to find the segment above the point
     */
    Segment* findAbove(int version, Point p) 
    {
        PNode* curr = root;
        Segment* result = nullptr;
        while(curr != nullptr)
        {
            int size = curr->nodes.size();
            auto it = upper_bound(curr->nodes.begin(), curr->nodes.end(),make_pair(version, reinterpret_cast<Node*>(~0ULL)));
            if (it == curr->nodes.begin()) 
            {
                throw runtime_error("No key less than k exists.");
            }
            --it;
            Node* node = it->second;
            Segment* seg = node->segment;
            double ycurr = node->segment->getY(p.x);
            if (p.y <= ycurr) {
                // Point is below or on current segment
                result = node->segment;
                curr = node->left;
            } else {
                // Point is above current segment
                curr = node->right;

            }
        }
        return result;
    }

    /**
     * Find the segment below a point in a specific version
     * @version: Version of the tree
     * @p: Point to be checked
     * This function traverses the tree to find the segment below the point
     */
    Segment* findBelow(int version, Point p) 
    {
        PNode* curr = root;
        Segment* result = nullptr;
        while(curr != nullptr)
        {
            int size = curr->nodes.size();
            auto it = upper_bound(curr->nodes.begin(), curr->nodes.end(),make_pair(version, reinterpret_cast<Node*>(~0ULL)));
            if (it == curr->nodes.begin()) 
            {
                throw runtime_error("No key less than k exists.");
            }
            --it;
            Node* node = it->second;
            Segment* seg = node->segment;
            double ycurr = node->segment->getY(p.x);
            if (p.y < ycurr) 
            {
                curr = node->left;
            } else {
                // Point is above current segment
                result = node->segment;
                curr = node->right;

            }
        }
        return result;
    }
};

/**
 * Comparator functions for sorting segments
 * compareByP1() sorts segments by their starting point (p1)
 * compareByP2() sorts segments by their ending point (p2)
 */
bool compareByP1(const Segment& a, const Segment& b) 
{
    if (a.p1.x != b.p1.x)
        return a.p1.x < b.p1.x;
    return a.p1.y < b.p1.y;
}

bool compareByP2(const Segment& a, const Segment& b) 
{
    if (a.p2.x != b.p2.x)
        return a.p2.x < b.p2.x;
    return a.p2.y < b.p2.y;
}

/**
 * PointLocation class
 * Contains a persistent tree and methods to locate segments above/below a point
 * locate() method finds the segment above and below a given point
 * Constructor initializes the persistent tree with segments
 */
class PointLocation 
{
private:
    PersistentTree* tree;      
    vector<Segment> start_segments; 
    vector<Segment> end_segments; 
    int sc;
    int ec;
    
public:
    /**
     * Constructor for PointLocation
     * Initializes the persistent tree with segments
     * @segments: Vector of segments to be added to the tree
     * This function sorts the segments and creates versions of the tree
     * It handles the insertion and deletion of segments based on their starting and ending points
     * It also creates slabs based on the x-coordinates of the segments
     * The slabs are used to determine the active segments in the tree
     * The constructor also initializes the x-coordinates vector
     * It removes duplicates and sorts the segments based on their starting and ending points
     */
    PointLocation(vector<Segment>& segments) 
    {
        tree = new PersistentTree();
        sc = 0;
        ec = 0;
        start_segments = segments;
        end_segments = segments;
       for (vector<Segment>::const_iterator it = segments.begin(); it != segments.end(); ++it) 
       {
            Segment seg = *it;
            x_coords.push_back(seg.p1.x);
            x_coords.push_back(seg.p2.x);
        }
        
        // Sort and remove duplicates
        sort(x_coords.begin(), x_coords.end());x_coords.erase(unique(x_coords.begin(), x_coords.end()), x_coords.end());
        sort(start_segments.begin(), start_segments.end(), compareByP1);
        sort(end_segments.begin(), end_segments.end(), compareByP2);

        // For each slab, determine active segments
        for (int i = 0; i < x_coords.size(); i++) 
        {
            double slab_left = x_coords[i];
            vector<Segment> add_segments;
            while(sc < start_segments.size() && start_segments[sc].p1.x == slab_left) {
                add_segments.push_back(start_segments[sc]);
                // cou
                sc++;
            }
            vector<Segment> remove_segments;
            while(ec < end_segments.size() && end_segments[ec].p2.x == slab_left) {
                remove_segments.push_back(end_segments[ec]);
                ec++;
            }
            tree->createVersion(add_segments, remove_segments,i);
        }
    }
    
    // Locate point - O(log² n)
    /**
     * Locate method
     * Finds the segment above and below a given point
     * @p: Point to be located
     * This function first finds the slab containing the point
     * Then it searches for the segments above and below the point
     * It returns a pair of segments (above, below)
     * It also prints the left and right boundaries of the slab
     * The function handles edge cases where the point is outside the bounds of the segments
     */
    pair<Segment*,Segment*> locate(const Point& p)
    {
        // Find slab containing point - O(log n)
        int slab = upper_bound(x_coords.begin(), x_coords.end(), p.x)-x_coords.begin();
        if(slab==0) 
        {
            out<<"Left "<<-100<<endl;
            out<<"Right "<<x_coords[slab]<<endl;
            return make_pair(nullptr, nullptr);
        }
        else if(slab==x_coords.size()) 
        {
            out<<"Left "<<x_coords[slab-1]<<endl;
            out<<"Right "<<100<<endl;
            return make_pair(nullptr, nullptr);
        }
        out<<"Left "<<x_coords[slab-1]<<endl;
        out<<"Right "<<x_coords[slab]<<endl; 
        // Search in appropriate tree version - O(log n)
        return make_pair(tree->findAbove(slab-1, p), tree->findBelow(slab-1, p));
    }
};
int main() {
    // Create test segments
    int n;
    cin >> n;
    vector<Segment> segments;
    for (int i = 0; i < n; i++)
    {
        double x1, y1, x2, y2;
        cin >> x1 >> y1 >> x2 >> y2;
        if (x1 > x2 || (x1 == x2 && y1 > y2)) {
            swap(x1, x2);
            swap(y1, y2);
        }
        xmin = min(xmin, x1);
        xmax = max(xmax, x2);
        ymin = min(ymin, y1);
        ymax = max(ymax, y2);
        segments.push_back(Segment(Point(x1, y1), Point(x2, y2), i));
    }
	for (const auto& seg : segments)
    {
        out << "SEG " << seg.p1.x << " " << seg.p1.y << " "<< seg.p2.x << " " << seg.p2.y << "\n";
    }
    PointLocation pl(segments);
    double xq,yq;
    cin>>xq>>yq;
    pair<Segment*,Segment*> result = pl.locate(Point(xq, yq));
    if (result.first != nullptr ) 
    {
        out << "Above "<<result.first->p1.x<<" "<<result.first->p1.y<<" "<< result.first->p2.x<<" "<<result.first->p2.y<< endl;
    }
    else
    {
        out << "Above -100 100 100 100 \n";
    }
    if(result.second != nullptr ) 
    {
        out << "Below "<<result.second->p1.x<<" "<<result.second->p1.y<<" "<< result.second->p2.x<<" "<<result.second->p2.y<< endl;
    } 
    else 
    {
        out << "Below -100 -100 100 -100 \n";
    }
    out << "QUERY " << xq << " " << yq << "\n";
    return 0;
}
