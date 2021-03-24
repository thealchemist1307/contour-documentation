///@file
///@brief This file contains a complete working implementation of R.H Guting's divide and conquer algorithm that helps solve the contour problem
#include<bits/stdc++.h>
using namespace std;
typedef long long int ll;
///Brief description of points class
///
///The point structure contains two integers, representing the x and y coordinates of the point in 2D space
typedef struct point
{
    ll x,y;
}point;

///Brief description of interval structure
///
///This structure contains two integers, representing the minimum coordinate and the maximum coordinate that is spanned by the interval.
typedef struct interval
{
    ///Represents the smaller coordinate
    ll bottom;
    ///Represents the larger coordinate    
    ll top;
    ///Definition of comparator to facilitate set insertions in a sorted order
    ///
    bool operator<(const interval& e) const
    { 
        if(this->bottom<e.bottom)
        {
            return true;
        }
        else if(this->bottom==e.bottom)
        {
            if(this->top<e.top)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
}interval;

///Brief description of line segment structure
///
///This structure contains a fixed coordinate x and an interval which represents the range of coordinates on the alternate axis from x
typedef struct line_segment
{
    ///Represents the range of coordinates on the alternate axis
	interval inter;
    ///Represents the fixed point on a certain axis
	ll x;
    ///Definition of comparator to facilitate set insertions in a sorted order
    ///
	bool operator<(const line_segment& e) const
    { 
        if(this->x<e.x)
		{
			return true;
		}
		else if(this->x==e.x)
		{
			if(this->inter.bottom<e.inter.bottom)
			{
				return true;
			}
			else if(this->inter.bottom==e.inter.bottom)
			{
				if(this->inter.top<e.inter.top)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
        else
        {
            return false;
        }
    }

}line_segment;

///Brief description of rectangle structure
///
///This structure contains two intervals that can define a rectangle along with a unique ID
typedef struct rectangle
{
    ///Interval data structure that contains the x-bounds of the rectangle 
    interval x_int;
    ///Interval data structure that contains the y-bounds of the rectangle
    interval y_int;
    ///Unique ID given in the order of input to identify each rectangle separately
    ll id;
    
}rectangle;

///Brief description of edge structure
///
///This structure contains an integer that defines a fixed point, an interval between which it spans, an edge type, and the unique ID representing the rectangle to which it belongs
typedef struct edge
{
    ///The type of the edge - either TOP, BOTTOM, LEFT or RIGHT
    string edgetype;
    ///The interval defines the length of the edge, and the minimum and maximum values that the variable coordinate can take
    interval i;
    ///The fixed coordinate of the edge
    ll x;
    ///Unique ID that was given to the rectangle containing this edge
    ll rect_id;
    ///Definition of comparator to facilitate set insertions in a sorted order
    ///
    bool operator<(const edge& e) const
    { 
        if(this->x < e.x)
        {
            return true;
        }
        else if(this->x == e.x)
        {
        
            if(this->edgetype == "LEFT" and e.edgetype == "RIGHT")
            {
                return true;
            }
            else if(this->edgetype == "RIGHT" and e.edgetype == "LEFT")
            {
                return false;
            }
            else
            {
                if(this->i.bottom < e.i.bottom)
                {
                    return true;
                }
                else if(this->i.bottom == e.i.bottom)
                {
                    if(this->i.top < e.i.top)
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
        } 
        else
        {
            return false;
        }
    }
}edge;

///Brief description of ctree structure
///
///Used to represent the x union datatype present in stripes data structure. Used to store a range of intervals in the form of a binary search tree
typedef struct ctree
{
    ///Represents the value of the node in the tree, used to decide whether to move to the left or the right of the tree during inorder traversal
    ll x;
    ///The type of the edge - either TOP, BOTTOM, LEFT or RIGHT
    string lru;
    ///The tree representing the left subtree of this node x
    ctree* lson;
    ///The tree representing the right subtree of this node x
    ctree* rson;
    ///Parameterized constructor to intiialize the values of the tree
    ctree(ll x, string lru, ctree* lson, ctree* rson)
    {
        this->x=x;
        this->lru=lru;
        this->lson=lson;
        this->rson=rson;
    }
}ctree;


///Brief description of stripe structure
///
///This structure is used to represent all horizontal stripes that the frame is partitioned into.
typedef struct stripe
{
    ///The interval x_int indicates the coordinates it spans from in the X direction
    interval x_int;
    ///The interval y_int indicates the coordinates it spans from in the Y direction
    interval y_int;
    ///Represents the binary search tree containing the x union information for this stripe
    ctree* tree;
    ///Definition of comparator to facilitate set insertions in a sorted order
    ///
    bool operator<(const stripe& e) const
    { 
        if(this->y_int.bottom<e.y_int.bottom)
        {
            return true;
        }
        else if (this->y_int.bottom==e.y_int.bottom)
        {
            if(this->y_int.top<e.y_int.top)
            {
                return true;
            }
            else 
            { 
                return false;
            }
        }
        else
        {
            return false;
        }
    }
}stripe;

///Global variable to store the possible values of edge types
string EDGE_TYPES[4] = {"TOP","BOTTOM","LEFT","RIGHT"};

///Global variable to store the subtree type
string LRU_TYPES[3] = {"LEFT","RIGHT","UNDEF"};

///Global variable to store the rectangles input by the user
vector<rectangle> iso_rectangles_input;

///Brief description of StripeReturnType
///
///This structure encompasses all the returned components from the stripes algorithm
typedef struct StripeReturnType
{
    ///contains the set of intervals on the left side of the median, along with the ID number of the edge that created the interval
    set<pair<interval,ll>> L;
    ///contains the set of intervals on the right side of the median, along with the ID number of the edge that created the interval
    set<pair<interval,ll>> R;
    ///contains the set of all points needed for partitioning
    set<ll> P;
    ///contains the set of stripes for the whole interval
    set<stripe> S;
}SRT;

/// \fn findFrame
///    Function that helps find the bounding frame around a set of iso-rectangles
///    \brief Finds bounding frame around set of rectangles
///    \return returns a rectangle structure containing information about the boundaries of the frame
///
rectangle findFrame()
{
    rectangle frame;
    ll minY=INT_MAX;
    ll minX=INT_MAX;
    ll maxY = INT_MIN;
    ll maxX = INT_MIN;
    for(ll i=0;i<iso_rectangles_input.size();i++)
    {
        rectangle r = iso_rectangles_input[i];
        if(r.x_int.bottom <= minX)
        {
            minX = r.x_int.bottom ;
        }
        if(r.x_int.top >= maxX)
        {
            maxX = r.x_int.top;
        }
        if(r.y_int.top >= maxY)
        {
            maxY = r.y_int.top;
        }
        if(r.y_int.bottom <= minY)
        {
            minY = r.y_int.bottom;
        }
    }
    interval x_bound;
    interval y_bound;
    x_bound.bottom = minX-2; x_bound.top = maxX+2;
    y_bound.bottom = minY-2; y_bound.top = maxY+2;
    frame.x_int = x_bound; frame.y_int = y_bound;
    return frame;
}

/*! Creates continuous intervals for a given set of points along Y axis
    \param y_coordinates a set of integers representing the coordinate points for partitioning
    \return Set of intervals representing the partition induced by y coordinates 
*/  
set<interval> y_partition(set<ll> y_coordinates)
{
    priority_queue <ll, vector<ll>, greater<ll>> partition_maker;
    set<interval> plane_partitions;
    set<ll>::iterator it = y_coordinates.begin();
    while(it != y_coordinates.end())
    {
        partition_maker.push(*it);
        ++it;
    }
    interval inter;
    ll prev = partition_maker.top();
    partition_maker.pop();
    while(!partition_maker.empty())
    {
        inter.bottom = prev;
        inter.top = partition_maker.top();
        prev = partition_maker.top();
        plane_partitions.insert(inter);
        partition_maker.pop();
    }
    return plane_partitions;
}
/*! Creates continuous intervals for a given set of points along Y axis
    \return Set of integers representing the unique y coordinates of vertices of the input iso rectangles
*/
set<ll> y_set()
{
    set<ll> y_coordinates;
    for(ll i=0;i<iso_rectangles_input.size();i++)
    {
        y_coordinates.insert(iso_rectangles_input[i].y_int.bottom);
        y_coordinates.insert(iso_rectangles_input[i].y_int.top);
    }
    return y_coordinates;
}

/*! Prints the inorder traversal of a binary search tree whose root is passed as the parameter
    \param node The root node of the tree of type ctree
*/
void inorder(ctree *node)
{
    if (node == NULL)
        return;
    if (node->lru == "LEFT" || node->lru == "RIGHT")
    {
        if (node->lru == "LEFT")
            cout << "Left: ";
        else
            cout << "Right: ";
        cout << " " << node->x << " ";
        return;
    }
    inorder(node->lson);
    inorder(node->rson);
}

/*! Creates a vector of pointers to trees containing the node whose values lie within the interval low_x and high_x
    \param node The root node of the tree of type ctree which has to be searched
    \param low_x Marks the lower limit of the interval to be searched
    \param high_x Marks the higher limit of the interval to be searched
    \param res A constantly updated vector of nodes of type ctree that are contained in the interval depicted by low_x and high_x
*/
void inorder_find(ctree *node, ll low_x, ll high_x, vector<ctree *> &res)
{
    if (node == NULL)
        return;
    if (node->lru == "LEFT" || node->lru == "RIGHT")
    {
            res.push_back(node);
    }
    if (node->x >= low_x)
        inorder_find(node->lson, low_x, high_x, res);
    if (node->x <= high_x)
        inorder_find(node->rson, low_x, high_x, res);
}
/*! Gets the intersection of two sets of intervals
    \param i1 defines a set of intervals, along with the rectangle ID associated with that interval
    \param i2 defines a set of intervals, along with the rectangle ID associated with that interval
    \return A set of pair of intervals and unique IDs constituting the set intersection of i1 and i2
*/
set<pair<interval,ll>> set_intersection(set<pair<interval,ll>> i1, set<pair<interval,ll>> i2)
{
    auto ite = i1.begin();
    auto itr = i2.begin();
    set<pair<interval,ll>> i3;
    while(ite!=i1.end() and itr!=i2.end())
    {
        interval a = (*ite).first;
        interval b = (*itr).first;
        ll a_id = (*ite).second;
        ll b_id = (*itr).second;
        if(a.bottom < b.bottom)
        {
            ite++;
        }
        else if(a.bottom == b.bottom)
        {
            if(a.top == b.top and a_id == b_id)
            {
                i3.insert({a,a_id}); 
                ite++;
                itr++;
            }
            else if(a.top == b.top and a_id != b_id)
            {
                if(a_id<b_id)
                {
                    ite++;
                }
                else
                {
                    itr++;
                }
            }
            else if(a.top < b.top)
            {
                ite++;
            }
            else
            {
                itr++;
            }
        }
        else
        {
            itr++;
        }
    }
    return i3;
}
/*! Gets the difference of two sets of intervals
    \param i1 defines a set of intervals, along with the rectangle ID associated with that interval
    \param i2 defines a set of intervals, along with the rectangle ID associated with that interval
    \return A set of pair of intervals and unique IDs constituting the set difference of i1 and i2
*/
set<pair<interval,ll>> set_difference(set<pair<interval,ll>> i1,set<pair<interval,ll>> i2)
{
    auto ite = i1.begin();
    auto itr = i2.begin();
    set<pair<interval,ll>> i3;
    while(ite!=i1.end() and itr!=i2.end())
    {
        interval a = (*ite).first;
        interval b = (*itr).first;
        ll a_id = (*ite).second;
        ll b_id = (*itr).second;
        if(a.bottom < b.bottom)
        {
            i3.insert({a,a_id}); 
            ite++;
        }
        else if(a.bottom == b.bottom)
        {
            if(a.top == b.top and a_id == b_id)
            {
                ite++;
                itr++;
            }
            else if(a.top == b.top and a_id != b_id)
            {
                if(a_id<b_id)
                {
                    i3.insert({a,a_id}); 
                    ite++;
                }
                else
                {
                    itr++;
                }
            }
            else if(a.top < b.top)
            {
                i3.insert({a,a_id});
                ite++;
            }
            else
            {
                itr++;
            }
        }
        else
        {
            itr++;
        }
    }

    while(ite != i1.end())
    {
        i3.insert((*ite));
        ite++;
    }

    return i3;
}
/*! Gets the union of two sets of intervals
    \param i1 defines a set of intervals, along with the rectangle ID associated with that interval
    \param i2 defines a set of intervals, along with the rectangle ID associated with that interval
    \return A set of pair of intervals and unique IDs constituting the set union of i1 and i2
*/
set<pair<interval,ll>> set_union(set<pair<interval,ll>> i1,set<pair<interval,ll>> i2)
{
    set<pair<interval,ll>> i3;
    auto ite = i1.begin();
    auto itr = i2.begin();
    while(ite != i1.end())
    {
        i3.insert(*ite);
        ++ite;
    }
    while(itr != i2.end())
    {
        i3.insert(*itr);
        ++itr;
    }
    return i3;
}
/*! Adds additional stripes to the existing set of stripes S based on the intervals induced by partition of P
    \param S defines a set of existing stripes
    \param P defines a set of partition points
    \param x_ext depicts the width of the stripe
    \return Updated set of stripes containing the additional stripes induced by partition of P
*/
set<stripe> copy(set<stripe> S, set<ll> P, interval x_ext)
{
    set<interval> part = y_partition(P);
    auto ite = part.begin();
    auto itr = S.begin();
    set<stripe> s1;
    while(ite != part.end())
    {
        stripe stemp;
        stemp.x_int = x_ext;
        stemp.y_int = (*ite);
        stemp.tree = NULL;
        
        while(itr!= S.end())
        {
            interval c = (*itr).y_int; //S
            if(stemp.y_int.bottom >= c.bottom)
            {
                if(stemp.y_int.top <= c.top)
                {
                    //useful S
                    stemp.tree = (*itr).tree;
                    break;
                }
                else
                {
                    itr++;
                }
            }
            else
            {
                break;
            }
        }

        s1.insert(stemp);
        ++ite;
    }

    return s1;

}
/*! Updates the x_union values for the stripes that have not found a partner edge in S
    \param S defines a set of existing stripes
    \param J defines the set of intervals which do not have a matching edge in S
    \return Modified set of stripes with updated x_union values
*/
set<stripe> blacken(set<stripe> S, set<pair<interval,ll>> J)
{
    auto itr = S.begin();
    auto ite = J.begin();
    set<stripe> s_final;
    while(itr != S.end())
    {
        stripe stemp = (*itr);
        interval i1 = stemp.y_int;
        while(ite != J.end())
        {
            //i1 should be subset of i2
            interval i2 = (*ite).first;
            if(i1.bottom < i2.bottom)
            {
                break;
            }
            else
            {
                if(i1.top <= i2.top)
                {
                    stemp.tree=NULL;
                    break;
                }
                else
                {
                    ite++;
                }
            }
        }
        s_final.insert(stemp);
        ++itr;
    }

    return s_final;
}
/*! Combines stripes located across the median line
    \param s_left defines a set of stripes on the left side of the median line
    \param s_right defines a set of stripes on the right side of the median line
    \param P defines a set of partition points
    \param x_ext depicts the width of the stripe
    \return Updated set of stripes formed using s_left and s_right after dissolving the median line 
*/
set<stripe> concat(set<stripe> s_left, set<stripe> s_right, set<ll> P, interval x_ext)
{
    set<interval> part = y_partition(P);
    auto ite = part.begin();
    auto itr1 = s_left.begin();
    auto itr2 = s_right.begin();
    set<stripe> S;

    while(ite != part.end())
    {
        stripe stemp;
        stemp.x_int = x_ext;
        stemp.y_int = (*ite);
        stemp.tree = NULL;

        stripe val1 = (*itr1);
        stripe val2 = (*itr2);
        
        if(val1.tree!=NULL and val2.tree!=NULL)  
        {
            stemp.tree=new ctree(val1.x_int.top,"UNDEF",val1.tree, val2.tree);
        }
        else if(val1.tree!=NULL and val2.tree==NULL)
        {
            stemp.tree=val1.tree;
        }
        else if(val1.tree==NULL and val2.tree!=NULL)
        {
            stemp.tree=val2.tree;
        }
        else
        {
            stemp.tree=NULL;
        }
        itr1++; 
        itr2++;
        S.insert(stemp);
        ++ite;
    }
    return S;
}
/*! Divde and conquer algorithm that computes the set of stripes along with their x_union values for the given set of vertical edges
    \param VRX defines a set of stripes on the left side of the median line
    \param x_ext depicts the width of the frame
    \param ylimits represents the height of the frame
    \return Intermediary results required for producing the result of the previous call (comprising of set of <interval,int> L, set of <interval,int> R, set of coordinates P and set of stripes S)
*/
SRT stripes(set<edge> VRX, interval x_ext, interval ylimits)
{
    
    if(VRX.size()==1)
    {
        SRT temp1;
        auto it = VRX.begin();
        edge e = *it;
        if(e.edgetype == "LEFT")
        {
            interval l1 = e.i;
            temp1.L.insert({l1,e.rect_id});
        }
        else
        {
            interval r1 = e.i;
            temp1.R.insert({r1,e.rect_id});    
        }

        temp1.P.insert(ylimits.bottom);
        temp1.P.insert(e.i.bottom);
        temp1.P.insert(e.i.top);
        temp1.P.insert(ylimits.top);

        set<interval> part = y_partition(temp1.P);
        auto ite = part.begin();
        while(ite != part.end())
        {
            stripe stemp;
            stemp.x_int = x_ext;
            stemp.y_int = *ite;
            stemp.tree =(ctree*)malloc(sizeof(ctree));
            stemp.tree=NULL;
            if((stemp.y_int.bottom == e.i.bottom) and (stemp.y_int.top == e.i.top))
            {
                if(e.edgetype == "LEFT")
                {
                    stemp.tree=new ctree(e.x,"LEFT",NULL,NULL);
                }
                else if(e.edgetype == "RIGHT")
                {
                    stemp.tree=new ctree(e.x,"RIGHT",NULL,NULL);
                }
            }
            temp1.S.insert(stemp);
            ++ite;
        }
        return temp1;
    }
    else
    {
        ll n = VRX.size();
        ll xm;
        set<edge> v1,v2;
        auto it = VRX.begin();
        for(ll i=0;i<n/2;i++)
        {
            v1.insert(*it);
            ++it;
        }
        for(ll i=n/2;i<n;i++)
        {
            if(i==n/2)
            {
                xm = (*it).x;
            }
            
            v2.insert(*it);
            ++it;
        }
        SRT left,right;
        interval left_call,right_call;
        left_call.bottom = x_ext.bottom;
        left_call.top = xm;
        right_call.bottom = xm;
        right_call.top = x_ext.top;

        left = stripes(v1, left_call,ylimits);
        right = stripes(v2,right_call,ylimits);

        
        set<pair<interval,ll>> L1=left.L;
        set<pair<interval,ll>> R2=right.R;
        
        set<pair<interval,ll>> L,R;
        set<pair<interval,ll>> LR;
        
        LR = set_intersection(L1,R2);
        set<pair<interval,ll>> l1lr = set_difference(L1,LR);
        set<pair<interval,ll>> r2lr = set_difference(R2,LR);
        L = set_union(right.L,l1lr);
        R = set_union(left.R,r2lr);
     
      
        set<ll> P;
        set<ll> P1= left.P;
        set<ll> P2= right.P;
        auto itr=P1.begin();
        while(itr!=P1.end())
        {
            P.insert(*itr);
            itr++;
        }
        itr=P2.begin();
        while(itr!=P2.end())
        {
            P.insert(*itr);
            itr++;
        }


        set<stripe> S_left,S_right;
        interval slefttemp;
        slefttemp.bottom=x_ext.bottom;
        slefttemp.top=xm;
        interval srighttemp;
        srighttemp.bottom=xm;
        srighttemp.top=x_ext.top;

        S_left=copy(left.S,P,slefttemp);
        S_right=copy(right.S,P,srighttemp);

        S_left = blacken(S_left,r2lr);
        S_right = blacken(S_right,l1lr);

        set<stripe> S;
        S = concat(S_left, S_right, P, x_ext);

        SRT temp2;
        temp2.L = L;
        temp2.R = R;
        temp2.P = P;
        temp2.S = S;
        
        return temp2;
    }
}
/*! Driver function used to call the divide and conquer algorithm stripes when provided with a set of iso-oriented rectangles
    \param rect defines the collection of iso oriented rectangles input by the users
    \return Final set of stripes computed by the stripes algorithm
*/
set<stripe> rectangle_DAC(vector<rectangle> rect)
{
    vector<rectangle>::iterator it = rect.begin();
    set<edge> VRX;
    while(it != rect.end())
    {
        edge e1;
        e1.edgetype = "LEFT";
        e1.i = (*it).y_int;
        e1.x =  (*it).x_int.bottom;
        // e1.rect=&(*it);
        e1.rect_id = (*it).id;


        edge e2;
        e2.edgetype = "RIGHT";
        e2.i = (*it).y_int;
        e2.x =  (*it).x_int.top;
        e2.rect_id = (*it).id;

        VRX.insert(e1);
        VRX.insert(e2);
        it++;
    }

    set<interval> L,R;
    set<ll> P;
    interval ylimits;
    rectangle frame = findFrame();
    ylimits.bottom = frame.y_int.bottom;
    ylimits.top = frame.y_int.top;
    interval xlimits;
    xlimits.bottom = frame.x_int.bottom;
    xlimits.top = frame.x_int.top;
    SRT srt;
    srt = stripes(VRX,xlimits, ylimits);
    return srt.S;
    
}
/*! Fumction to compute the horizontal contours associated with the given edge and the set of stripes
    \param h Horizontal edge which is part of an input rectangle
    \param S defines the set of stripes that the frame is partitioned into
    \return Set of line segments representing the horizontal contours associated with the current edge
*/
set<line_segment> contour_pieces(edge h, set<stripe> S)
{
	
    stripe stemp;
    if(h.edgetype=="TOP")
    {
        for(auto t : S)
        {
            if(t.y_int.bottom == h.x)
            {
                stemp = t;
                break;
            }
        }
    }
    else
    {
        for(auto t : S)
        {
            if(t.y_int.top == h.x)
            {
                stemp = t;
                break;
            }
        }
    }
    

    ll lower_X = h.i.bottom;
    ll higher_X = h.i.top;
    vector<ctree *> result;
    set<interval> J;
    inorder_find(stemp.tree, lower_X, higher_X, result);
    
    ll pointX = lower_X;
    bool op = true;
    for (auto it : result)
    {
        if (it->lru == "LEFT")
        {   
            if (pointX < it->x)
                J.insert({pointX, min(higher_X,it->x)});
            op = false;
        }
        else if(it->x>=pointX)
        {
            pointX = it->x;
            op = true;
        }
    }

    if (op)
    {
        if (pointX < higher_X)
        {
            J.insert({pointX, higher_X});
        }
    }
    set<line_segment> resultant_lines;
    ll y = h.x;
    for (auto it : J)
    {
        line_segment ls;
        ls.inter = it;
        ls.x = y;
        resultant_lines.insert(ls);
    }

    // cout << "Horizontal edge is : " << endl;
    // cout << h.i.bottom << " " << h.i.top << " " << h.x << endl;
    
    // for (auto it : result)
    // {
    //     cout << it->x << " " << it->lru << endl;
    // }
    // cout << endl;

    return resultant_lines;
}

/*! Function that computes all the horizontal contours generated using the horizontal edges from the input rectangles and the set of stripes
    \param H Set of horizontal edges formed by the input rectangles
    \param S defines the set of stripes that the frame is partitioned into
    \return Set of line segments representing the horizontal contours for the union of input rectangles
*/
set<line_segment> contour(set<edge> H, set<stripe> S)   
{
	set<line_segment> ans_contour;

	auto it=H.begin();
	while(it!=H.end())
	{
		edge temp=(*it);
        // cout<<"The horizontal edge is : "<<temp.x<<" "<<temp.i.bottom<<" "<<temp.i.top<<endl;
		set<line_segment> ltemp=contour_pieces(temp,S);
		auto it1=ltemp.begin();
        while(it1!=ltemp.end())  
        {
            ans_contour.insert(*it1);
            it1++;
        }
		it++;
	}
	return ans_contour;
}
/*! Function that merges a set of overlapping intervals 
    \param s Set of intervals to be merged 
    \return Set of intervals containing intervals without overlap formed by merging the overlapping intervals in s
*/
set<interval> merge_intervals(set<interval> s)
{
    set<interval> ans;
    interval itemp;
    auto it = s.begin();
    itemp.bottom = (*it).bottom;
    itemp.top = (*it).top;
    ++it;
    while(it!=s.end())
    {
        interval cur = *it;
        if(cur.bottom <= itemp.top)
        {
            itemp.top = max(cur.top,itemp.top);
        }
        else 
        {
            ans.insert(itemp);
            itemp.bottom = cur.bottom;
            itemp.top = cur.top;
        }
        ++it;
    }
    ans.insert(itemp);
    return ans;
}
/*! Function that writes the coordinates of the line segments that form the horizontal and vertical contours, along with the input rectangles into separate files for later use in visualization
    \param hrc Set of horizontal line segments that form the horizontal contours
    \param vrc Set of vertical line segments that form the vertical contours
*/
void csv_writer(set<line_segment> hrc, set<line_segment> vrc)
{
    std::ofstream myFile("rectangles.csv");
    
    
    myFile << "Rectangles\n";
    rectangle frame = findFrame();
    myFile<< frame.x_int.bottom<<", "<<frame.x_int.top<<", "<<frame.y_int.bottom<<", "<<frame.y_int.top<<"\n";
    auto iter = iso_rectangles_input.begin();
    while(iter != iso_rectangles_input.end())
    {
        rectangle r = (*iter);
        myFile<< r.x_int.bottom<<", "<<r.x_int.top<<", "<<r.y_int.bottom<<", "<<r.y_int.top<<"\n";
        
        iter++;
    }
    
    myFile.close();

    std::ofstream myFile2("contours.csv");
    

    myFile2 << "Contours\n";
    auto itc = hrc.begin();
    while(itc != hrc.end())
    {
        line_segment ls = (*itc);
        myFile2<< ls.inter.bottom<<", "<<ls.x<<", "<<ls.inter.top<<", "<<ls.x<<"\n";
        itc++;
    }
    auto itv = vrc.begin();
    while(itv != vrc.end())
    {
        line_segment ls = (*itv);
        myFile2<< ls.x<<", "<<ls.inter.bottom<<", "<<ls.x<<", "<<ls.inter.top<<"\n";
        itv++;
    }
    
    myFile2.close();
}

int main()
{
   
    ll n; 
    cin>>n;
    interval x,y;
    ll x1,x2,y1,y2;
    for(ll i=0;i<n;i++)
    {
        cin>>x1>>x2>>y1>>y2;
        x.bottom = x1; 
        x.top = x2;
        y.bottom = y1;
        y.top = y2;
        rectangle r;
        r.x_int = x;
        r.y_int = y;
        r.id = i;
        iso_rectangles_input.push_back(r);
    }
    rectangle frame = findFrame();
   
    
    set<stripe> S = rectangle_DAC(iso_rectangles_input);
   

    vector<rectangle>::iterator it = iso_rectangles_input.begin();
    set<edge> HRX;
    while(it != iso_rectangles_input.end())
    {
        edge e1;
        e1.edgetype = "BOTTOM";
        e1.i = (*it).x_int;
        e1.x =  (*it).y_int.bottom;
        e1.rect_id=(*it).id;

        edge e2;
        e2.edgetype = "TOP";
        e2.i = (*it).x_int;
        e2.x =  (*it).y_int.top;
        e2.rect_id=(*it).id;

        HRX.insert(e1);
        HRX.insert(e2);
        it++;
    }
    set<line_segment> horizontal_contours_unmerged = contour(HRX,S);
    


    map<ll, set<interval>> merged_horizontal_contours;
    for (auto h : horizontal_contours_unmerged)
    {
        interval i1;
        i1.bottom = h.inter.bottom;
        i1.top = h.inter.top;
        merged_horizontal_contours[h.x].insert(i1);
    }

    

    vector<pair<ll,ll>> vertical_contours_points;

    for(auto h : merged_horizontal_contours)
    {
        set<interval> s1 = merge_intervals(h.second);
        merged_horizontal_contours[h.first] =  s1;
        for(auto siter : s1)
        {
            interval temp = siter;
            pair<ll,ll> p1;
            p1.first = temp.bottom;
            p1.second = h.first;
            vertical_contours_points.push_back(p1);
            p1.first = temp.top;
            vertical_contours_points.push_back(p1);
        }
    }
    set<line_segment> horizontal_contours;


    
    for (auto h : merged_horizontal_contours)
    {
        for(auto val : h.second)
        {
            line_segment ls;
            ls.inter.bottom = val.bottom;
            ls.inter.top = val.top;
            ls.x = h.first;
            horizontal_contours.insert(ls);
            
        }
    }
    cout << "Horizontal Contours\n";
    for(auto lsi : horizontal_contours)
    {
        cout<<lsi.inter.bottom<<" "<<lsi.x<<"     "<<lsi.inter.top<<" "<<lsi.x<<endl;
    }
    sort(vertical_contours_points.begin(),vertical_contours_points.end());
    
    set<line_segment> vertical_contours;
    auto vit = vertical_contours_points.begin();
    cout<<"Vertical Contours\n";
    while(vit!= vertical_contours_points.end())
    {
        pair<ll,ll> p1;
        p1.first = (*vit).first; 
        p1.second = (*vit).second;
        line_segment l1;
        l1.x = p1.first;
        l1.inter.bottom = p1.second;
        vit++;
        p1.second = (*vit).second;
        l1.inter.top = p1.second;
        vertical_contours.insert(l1);
        vit++;           
        cout<<l1.x<<" "<<l1.inter.bottom<<"     "<<l1.x<<" "<<l1.inter.top<<endl;

    }

    csv_writer(horizontal_contours,vertical_contours);
}