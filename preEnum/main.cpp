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
    Point();
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

Point::Point()
{
    x = -1;
    y = -1;
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
    void updateDist();
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
    float checkCrossed(Edge &ii, Edge &xx);
    void twoOpt();
    
    Point* arrayMaker();
    float arrayDistCalc(Point* array);
    float brutish(float &greedy);
    void arraySwap(int &xx, int &ii, Point* greedArray);
    
};//END tsp declaration

tsp::tsp()
{
    ifstream inFile("//Users//amymcneill//Desktop//tspV2//input_6.csv");
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
//        twoOpt();
        
        float brutishClose = brutish(greedBest);
        
        cout << "Brutish produces: " << brutishClose << endl;
        
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

void Edge::updateDist()
{
    this->distance = distCalc(left, right);
}

void tsp::swap(Edge &first, Edge &second)
{
    cout << first.left->x << "  " << first.right->x << endl;
    cout << second.left->x << "  " << second.right->x << endl;
    
    Point* FL = first.left;
    Point* FR = first.right;
    Point* SL = second.left;
    Point* SR = second.right;

    first.left = FL;
    first.right = SR;
    
    second.left = SL;
    second.right = FR;
    
    first.updateDist();
    second.updateDist();
    
//    cout << first.distance << "  " << second.distance << endl;
}//END swap

float tsp::checkCrossed(Edge &ii, Edge &xx)
{
    Edge c, d;
    
    c.left = ii.left;
    c.right = xx.right;
    d.left = xx.left;
    d.right = ii.right;
    
    return distCalc(c.left, c.right) + distCalc(d.left, d.right);
}//END check crossed function

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
    
    for(int ii = 0; ii < ceil(pointNum/2); ii++)
    {
        for(int xx = ii + 1; xx < pointNum; xx++)
        {
            if (checkCrossed(edges[ii], edges[xx]) < (edges[ii].distance + edges[xx].distance))
            {
//                swap(edges[ii], edges[xx]);
                cout << "Swapping!" << endl;
            }
        }
    }
    
    float best = currentDist();
    
    cout << "The shortest possible distance is " << best << endl;
    
    delete edges;
    
}//END twoOpt function

Point* tsp::arrayMaker()
{
    Point* array = new Point[pointNum];
    Point* temp = start;
    int ii = 0;
    
    while(temp != end)
    {
        array[ii] = *temp;
        temp = temp->next;
        ii++;
    }
    array[pointNum - 1] = *end;
    return array;
}

void tsp::arraySwap(int &xx, int &ii, Point* greedArray)
{
    Point xholder = greedArray[xx];
    greedArray[xx] = greedArray[ii];
    greedArray[ii] = xholder;
}

float tsp::arrayDistCalc(Point* array)
{
    float dist = 0.0;
    
    for (int ii = 0; ii < pointNum -1; ii++)
    {
        dist+=distanceCalc(&array[ii], &array[ii+1]);
    }
    dist += distanceCalc(&array[0], &array[pointNum - 1]);
    
    return dist;
}

float tsp::brutish(float &greedy)
{
    float newDist = greedy;
    Point* array = arrayMaker();
    
    for(int ii = 0; ii < pointNum; ii++)
    {
        for (int xx = ii; xx < pointNum; xx++)
        {
            arraySwap(xx, ii, array);
            if(arrayDistCalc(array) > newDist)
                arraySwap(xx, ii, array);
            else
            {
                cout << "Swap is sucessful!" << endl;
                newDist = arrayDistCalc(array);
            }
        }
    }
    
    
    return newDist;
    
}

int main()
{
    tsp myProblem;
    return 0;
}
