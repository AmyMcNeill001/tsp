#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
using namespace std;

/****************************************************************************
 *                                                                          *
 *               struct Point Declaration & Definitions                     *
 *                                                                          *
 ****************************************************************************/

struct Point
{
    float x;
    float y;
    Point* next;
    Point* previous;
    Point(float &inX, float &inY, Point* inNext, Point* inPrevious);
    Point& operator=(Point &rhsObj);
};//END struct Point

Point::Point(float &inX, float &inY, Point* inNext, Point* inPrevious)

{
    x = inX;
    y = inY;
    next = inNext;
    previous = inPrevious;
}//END point constructor with parameters

Point& Point::operator=(Point &rhsObj)
{
    x = rhsObj.x;
    y = rhsObj.y;
    
    next = rhsObj.next;
    previous = rhsObj.previous;
    
    return *this;
}

/****************************************************************************
 *                                                                          *
 *                struct Edge Declaration & Definitions                     *
 *                                                                          *
 ****************************************************************************/

struct Edge
{
    Point* left;
    Point* right;
    float distance;
    
    Edge();
    Edge(Point* inLeft, Point* inRight);
    Edge& operator=(Edge &rhsObj);
};//END struct Edge

float distCalc(Point* pointOne, Point* pointTwo)
{
    return sqrt(pow((pointOne->x-pointTwo->x), 2.0) + pow((pointOne->y-pointTwo->y), 2.0));
}//END function distCalc

Edge::Edge()
{
   //nothing to do here
}//END edge default constructor

Edge::Edge(Point* inLeft, Point* inRight)
{
    left = inLeft;
    right = inRight;
    distance = distCalc(left, right);
}//END edge constructor

Edge& Edge::operator=(Edge &rhsObj)
{
    this->left = rhsObj.left;
    this->right = rhsObj.right;
    this->distance = rhsObj.distance;
    
    return *this;
}
/****************************************************************************
 *                                                                          *
 *                tsp Class Declaration & Definitions                       *
 *                                                                          *
 ****************************************************************************/


class tsp
{
public:
    //for the initial info in greedy
    Point* first;
    Point* last;
    
    //for the list of reached points used in greedy
    Point* start;
    Point* end;
    
    //function list for greedy
    Point *closest(Point &current);
    float distanceCalc(Point* firstPoint, Point* secondPoint);
    void add(float &inX, float&inY);
    void reached(Point &inPoint);
    float greedy();
    
    //to keep track of the number of points
    int pointNum;
    
    //tsp constructor and destructor
    tsp();
    ~tsp();
    
    //members and methods for twoOpt
    Edge* edges;
    void swap(Edge &first, Edge &second);
    float currentDist();
    void twoOpt();
};//END tsp declaration

tsp::tsp()
{
    ifstream inFile("//Users//amymcneill//Desktop//tspV2//input_4.csv");
    if (inFile)
    {
        cout << "It's opened!" << endl;
        
        //read in first line
        string dummy = "";
        getline(inFile, dummy);
        
        //read in each coordinate pair
        string line;
        float x,y;
        char delimiter;
        while(getline(inFile, line))
        {
            istringstream instring(line);
            instring >> x >> delimiter >> y;
            cout << "New Point: " << x << ", " << y << endl;
            pointNum++;
            add(x,y);
        }//END while loop to read in coordinate pairs
        
        start = end = NULL;
        
        float greedBest = greedy();
        cout << "The best distance is: " << greedBest << " meters!" << endl;
        twoOpt();
        
        inFile.close();
    }
    else
        cout << "The input file failed to open!" << endl;
}//END tsp class constructor

float tsp::distanceCalc(Point* firstPoint, Point* secondPoint)
{
    return sqrt(pow((firstPoint->x-secondPoint->x), 2.0) + pow((firstPoint->y-secondPoint->y), 2.0));
}//END distance function def

void tsp::add(float &inX, float &inY)
{
    if(first == NULL)
    {
        first = last = new Point(inX, inY, NULL, NULL);
    }
    else
    {
        
        //create a temporary pointer to traverse the linked list
        Point* temp = this->first;
        
        //traverse the list as long as
        while (temp->next != NULL)
        {
            temp = temp->next;
        }//END list traversal to get to tail
        
        temp->next = new Point(inX, inY, NULL, temp);
        last = temp->next;
    }
}//END add function

void tsp::reached(Point &inPoint)
{
    //if we're starting the reached list
    if(start == NULL)
    {
        start = first;
        first = first->next;
        first->previous = NULL;
        start->next = NULL;
        start->previous = NULL;
        end = start;
    }
    //if it's the last node to be reached
    else if (last->previous == NULL && &inPoint == last)
    {
        end->next = &inPoint;
        inPoint.previous = end;
        end = end->next;
        first = last = NULL;
    }
    //all other situations
    else
    {
        //if it's the first node on the list
        if(first == &inPoint)
        {
            end->next = first;
            first = first->next;
            first->previous = NULL;
            end = end->next;
            end->next = NULL;
        }
        //if it's the last node on the list
        else if (last == & inPoint)
        {
            end->next = &inPoint;
            last = last->previous;
            last->next = NULL;
            inPoint.previous = end;
            end = end->next;
        }
        //if it's a middle node
        else
        {
            end->next = &inPoint;
            inPoint.previous->next = inPoint.next;
            inPoint.next->previous = inPoint.previous;
            inPoint.previous = end;
            end = end->next;
            end->next = NULL;
        }
        
    }
}//END reached function

Point* tsp::closest(Point &current)
{
    Point* temp = &current;
    Point* currentClose = NULL;
    
    //special situation where last node on initial list
    if (temp == last && temp == first)
    {
        first = NULL;
        return start;
    }
    
    //if point being analyzed is the start point
    else if(temp == first)
    {
        float closestDist = distanceCalc(&current, temp->next);
        currentClose = temp->next;
        temp = temp->next;
        
        while (temp->next != NULL)
        {
            if (distanceCalc(&current, temp->next) < closestDist)
            {
                closestDist = distanceCalc(&current, temp->next);
                currentClose = temp->next;
            }
            
            temp = temp->next;
        }
        
        return currentClose;
    }
    
    else if (temp == last)
    {
        float closestDist = distanceCalc(&current, temp->previous);
        currentClose = temp->previous;
        temp = temp->previous;
        
        while (temp->previous != NULL)
        {
            if (distanceCalc(&current, temp->previous) < closestDist)
            {
                closestDist = distanceCalc(&current, temp->previous);
                currentClose = temp->previous;
            }
            
            temp = temp->previous;
        }
        
        return currentClose;
    }
    
    else
    {
        float closestDist = distanceCalc(&current, temp->next);
        currentClose = temp->next;
        temp = temp->next;
        
        while (temp ->next != NULL)
        {
            if (distanceCalc(&current, temp->next) < closestDist)
            {
                closestDist = distanceCalc(&current, temp->next);
                currentClose = temp->next;
            }
            
            temp = temp->next;
        }
        
        temp = &current;
        
        while(temp->previous != NULL)
        {
            if (distanceCalc(&current, temp->previous) < closestDist)
            {
                closestDist = distanceCalc(&current, temp->previous);
                currentClose = temp->previous;
            }
            
            temp = temp->previous;
        }
        
        return currentClose;
    }
    
}//END closest function
float tsp::greedy()
{
    float bestDist = 0.0;
    
    Point* temp = first;
    while ( temp != start)
    {
        Point* myClosest = closest(*temp);
        float tempNum = distanceCalc(temp, myClosest);
        bestDist = bestDist + tempNum;
        reached(*temp);
        temp = myClosest;
    }
    
    return bestDist;
}//END greedy function

tsp::~tsp()
{
    int deleteCount = 0;
    Point* temp = start;
    while(start != end)
    {
        start = start->next;
        delete temp;
        temp = temp->next;
        deleteCount++;
    }
    delete end;
    deleteCount++;
    
    cout << deleteCount << " points were deallocated! " << endl;
}//END tsp destructor

float tsp::currentDist()
{
    float dist = 0.0;
    for(int ii = 0; ii < pointNum; ii++)
    {
        dist += edges[ii].distance;
    }
    return dist;
}//END currentdist

void tsp::swap(Edge &first, Edge &second)
{
    Edge tempOne = Edge(first.left, second.right);
    Edge tempTwo = Edge(second.left, first.right);
    
    first = tempOne;
    second = tempTwo;
}//END swap

void tsp::twoOpt()
{
    
    //populate array of edges
    edges = new Edge[pointNum];
    Point* temp = start;
    
    for(int ii = 0; ii < pointNum-1; ii++)
    {
        Edge edgeHolder(temp, (temp->next));
        edges[ii] = edgeHolder;
        temp = temp->next;
    }
    
    Edge edgeHolder(end, start);
    edges[pointNum-1] = edgeHolder;
    
    //perform two-opt comparisons
    for(int ii = 0; ii < ceil(pointNum/2); ii++)
    {
        float origDist = currentDist();
        
        if(ii == 0)
        {
            for(int xx = 2; xx < pointNum - 2; xx++)
            {
                //try swapping this edges
                swap(edges[ii], edges[xx]);
                
                //if it's not better, switch back
                if(currentDist() > origDist)
                {
                    cout << "Swapping back" << endl;
                    swap(edges[ii], edges[xx]);
                }
                else
                    origDist = currentDist();
            }
        }
        
    }//END outer loop through first half of nodes
    
    float best = currentDist();
    
    cout << "The shortest possible distance is " << best << endl;
    
    delete edges;
    
}//END twoOpt function

int main()
{
    tsp myProblem;
    return 0;
}
