#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>
#include <queue>

using namespace cv;
using namespace std;


/*
 * COMP 6651 P#1: Vigneswar Mourouguessin  
 *
 */


int Threshold = 0.5;
int c;
bool Marker = false;
int max_val = 3000000;
ofstream output("output.txt");

struct axis {
  int x_axis;
  int y_axis;
  string type;

};


//Add vector weights to paths
void Add_val_cost(vector<vector<pair<int, int> > > &adj, int row, int col, int value) {
	 
    int counter=0;	 
    vector<pair<int,int> >  &vec = adj.at(row);
    bool flag=false;
   
    if(value == 3)
    {
    ::c++;
    }
    vec.push_back(make_pair(col,value));
    counter++;
 
}


//Setting Foreground weights
void setForeGround(vector < int > &src,vector<axis> &ForeGround,int width,vector< vector<pair<int,int> > > &adj,int Pixel_count)
{
  int vals=0;
  int position=1;
  for (int index = 0; index < ForeGround.size() ; ++index) 
  {
      vals=0;
      vals=(ForeGround.at(index).y_axis*width)+(ForeGround.at(index).x_axis)+position;
      src.push_back(vals);
      Add_val_cost(adj,vals,Pixel_count-1,3000000);
  }
 }

//Setting Background weights
void setBackground(vector < int > &des,vector<axis> &Background,int width,vector< vector<pair<int,int> > > &adj,int Pixel_count)
{
  int vals=0;
  int position=1;
  for (int index = 0; index < Background.size() ; ++index)
  {
    vals=0;
    vals=(Background.at(index).y_axis*width)+(Background.at(index).x_axis)+position;
    des.push_back(vals);
    Add_val_cost(adj,0,vals,3000000);
  }	
}

//Finding Match
int find_Val(vector<vector<pair<int,int> > > &adj,int row, int col,Mat &out){

     int val =0;
     int count=0;
     bool flag=false;
     vector<pair<int,int> > &vec = adj.at(row);
        
        for(int index=0; index<vec.size();index++){
            pair<int,int> &item = vec.at(index);
            count++;
            if(item.first == col){
                val= item.second;
                flag=true;
            }
        }
        
    return val;
}

//Reset Graph
void Reset_vector(vector < pair < int, int > > &R_adj,int N)
{
  R_adj.clear();
  for (int i = 0; i < N; i++)
  {
    pair < int, int > npair = make_pair(i, 0);
    R_adj.push_back(npair);
  }
}


//Adding source
void Add_Source(queue < vector < int > > &queue,vector < int > &src,int N)
{
for (int i = 0; i < N; i++)
 {
  vector < int > route_0;
  route_0.push_back(src[i]);
  queue.push(route_0);
  }
  ::Marker=true;
}

//Resting Queue
void Empty_queue(queue<int> &queue)
{
    while (!queue.empty()) {
        queue.pop();
      }

}

//Update Values
void update_Val(vector<vector<pair<int, int> > > &adj, int row, int col, int updateValue,Mat &out) {
   
   vector<pair<int,int> > &vec = adj.at(row);
   int count=0;
   bool flag=false;
   
        for(int index=0; index<vec.size();index++){
            pair<int,int> &item = vec.at(index);
            if(item.first == col){
                count++;
                vec.at(index) = make_pair(col,updateValue);
                flag=true;
            }
        }
}


//Printing Graph
void print_vector(vector< vector<pair<int,int> > > &adj)
{
  //output << adj.size()<<"\n";
  int counter =0;
  for( int index = 0; index < adj.size(); index++ )
    {
          vector<pair<int,int> > &vector = adj.at(index);
     
          for(int j=0; j<vector.size();j++)
          {
               pair<int,int> &pos = vector.at(j);
	
               if(pos.second==1)
               {	
		    counter++;
               }
          }
      }
 
}


//Finding the number of pixels in the given image
int Pixelcount(Mat Blur,int width,int height){
int Pixelcount = 0;
for (int i = 0; i < height; i++) {
	    for (int j = 0; j < width; j++) {
	      Pixelcount++;
	    }
	  }
return Pixelcount;
}

//finding Route
bool Search_route(vector<vector<pair<int,int> > > &rAdj, int source, int target, int *parent, int V,Mat &out) {
    
    bool *Rechable= new bool[V]();
    queue<int> Vqueue;
    int temp;
    int f,item,queue_C;
    vector<pair<int,int> > vectorPairs;
    pair<int,int> pairs;
    queue<int> Search_queue;
    Vqueue.push(source);
    Rechable[source] = true;
    parent[source] = -1;
   
    
    if(!(::Marker))
   { 
    while (!Vqueue.empty()) 
    {
        item = Vqueue.front();
        Vqueue.pop();
        vectorPairs = rAdj.at(item);
        
        for (int i=0; i<vectorPairs.size(); i++) 
        {
            pairs = vectorPairs.at(i);
            f= pairs.first;
            Search_queue.push(f);
            
            if(!Rechable[f])
              { 
                 temp = find_Val(rAdj,item,f,out);
		    if (temp > 0) 
		    {
		        parent[f] = item;
		        Rechable[f] = true;
		        Vqueue.push(f);
		        queue_C++;
		    }
             } 
        }
        
        if(Rechable[target])
            break;
    }

  }
    Empty_queue(Search_queue);
    return Rechable[target];
}

//Finding min
int getMinval(int min_val,int temp,int count)
{

  count++;
  if(min_val < temp)
  {
    return min_val;
  }
  else
  {
  return temp;
  }

}


//Update RGraph
void Update_Residual(int sink,int source,int *route,int min_val,vector<vector<pair<int,int> > > &rAdj,int count,Mat &out)
{
  int v,u,temp,Value;
  bool flag=false;
  
   for (v = sink; v != source; v = route[v])
         {
            u = route[v];
            
            //Update source to sink
            temp = find_Val(rAdj,u,v,out);
            Value = temp - min_val;
            update_Val(rAdj,u,v,Value,out);
            count++;
            
            //update sink to source
            temp = find_Val(rAdj,v,u,out);
            Value = temp + min_val;
            update_Val(rAdj,v,u,Value,out);
            count++;
            
            flag=true;
        }	

}


//Coloring Output
void Set_Out_Image(int V,Mat &out,bool *Rechable)
{

    int x_axis, y_axis;
    int width = out.cols;
    int bcount=0,fcount=0;
    
    for (int index = 1; index < V; ++index) 
    {
        x_axis = (index-1) / width;
        y_axis = (index-1) % width;
       
	    if (Rechable[index] == true) 
	    {
	    //BackGround b-g-r
	    Vec3b v1;
	    v1[0] = 0; 
	    v1[1] = 0; 
	    v1[2] = 0; 
	    bcount++;
	    out.at<Vec3b>(x_axis, y_axis) = v1;
	    } 
	    else
	    {
	    //ForeGround b-g-r
	    Vec3b v2;
	    v2[0] = 255; 
	    v2[1] = 255; 
	    v2[2] = 255; 
	    fcount++;
	    out.at<Vec3b>(x_axis, y_axis) = v2;
	    }
    }

}


//finding Max
int FindMax(Mat Blur,int width,int height,int Pixel_count){

   int marker=0;	
   for(int id = 0; id < Pixel_count; id++)
     {
      int current = Blur.at < uchar > (id / width, id % width);

      if (marker < current) {
        marker = current;
	}
     
     }	
  return marker	;
}


//Perform Operations
void Compute(vector<vector<pair<int,int> > > &adj, int source, int sink, int V, Mat &out) 
{
    int u, v,temp;
    int count = 0;
    vector<vector<pair<int,int> > > rAdj(adj);
    int *route = new int[V];
    int Value=0;
    int queue_C=0;
    
    bool *Rechable = new bool[V]();
    queue<int> Vqueue;
    int item,f;
    vector<pair<int,int> > vectorPairs;
    pair<int,int> Vpair;
    queue<int> Search_queue;
    
    if(::Marker)
    {
     //Inside Search route 
      while ( ::Marker )
     {
        ::Marker = false;
        
        while (Search_route(rAdj, source, sink, route, V,out)) 
        {
        
        int min_val = ::max_val;
        for (v = sink; v != source; v = route[v]) 
        {
            u = route[v];
            temp= find_Val(adj,u,v,out);
            min_val = getMinval(min_val, temp,count);
            
        }

	Update_Residual(sink,source,route,min_val,rAdj,count,out);
	count++;
        ::Marker=true;
      }
    }
    
 //Finding Reachable elements
    Vqueue.push(source);
    Rechable[source] = true;
    
   if(!(::Marker))
   {  
    while (!Vqueue.empty()) 
    {
      
        item = Vqueue.front();
        Vqueue.pop();
        vectorPairs = rAdj.at(item);
        
        for (int i=0; i<vectorPairs.size(); i++) 
        {
            Vpair = vectorPairs.at(i);
            f= Vpair.first;
            Search_queue.push(f);
           
           if(!Rechable[f] )
           {
               temp = find_Val(rAdj,item,f,out) ;	
           
            if (temp> 0)
             {
            
                Rechable[f] = true;
                Vqueue.push(f);
                queue_C++;
            }
          } 
       }   
     }
    
    }

    //Setting Output   
    Set_Out_Image(V,out,Rechable);
    
    //reset values
    delete route;
    Empty_queue(Vqueue);
    delete Rechable;
    Empty_queue(Search_queue);
    return;
  }

}


//assign weights
void check_level(int out,int pixel,int pixel2,vector< vector<pair<int,int> > > &adj)
{
  if(out==(::Threshold))
    {
    Add_val_cost(adj,pixel,pixel2,3000000);
    }
   else
     {
   Add_val_cost(adj,pixel,pixel2,3);
      }
}


void UpdateWeights(vector<vector<pair<int,int> > > &adj, int height, int width, Mat &Blur) 
{
    
    int position =1;
    int counter = 0;
    int Pixel_count = height*width ;	

    for (int i = 0; i < height; i++)
     {
     for (int j = 0; j < width; j++)
     {
      int pixel = position + i * width + j ;
      int pixel2=0;
      counter++;
 
      //check pixelleft
      if(j-1>=0)
      {
        //output<< "Inside left---";
        pixel2 = position+(i * width + (j - 1));
        int out = (Blur.at<uchar>(i,j) - Blur.at<uchar>(i, j-1));
        check_level(out,pixel, pixel2 ,adj);
      }

      //check pixelright
      if(j+1<width)
      {
        //output<< "Inside right---";
        pixel2= position + (i * width + (j + 1));
        int out = (Blur.at<uchar>(i,j) - Blur.at<uchar>(i, j+1));
        check_level(out,pixel, pixel2 ,adj);
      }

      //check pixeltop
      if(i-1>=0)
      {  // output<< "Inside top---";
      	pixel2= position+((i - 1) * width + j);
      	int out = (Blur.at<uchar>(i,j) - Blur.at<uchar>(i-1, j));
        check_level(out,pixel, pixel2 ,adj);
      }

      //check piextbottom
      if(i+1<height)
      {
        //output<< "Inside bottom---";
        pixel2= position+((i + 1) * width + j);
        int out = (Blur.at<uchar>(i,j) - Blur.at<uchar>(i+1, j));
        check_level(out,pixel, pixel2 ,adj);
      }

   }
   
   }
 
}


//Storing values from Config
void getInput(int x, int y, vector<axis> &ForeGround,vector<axis> &Background,int value)
{
  axis ax;
  if(value==1)
	{
	ax.x_axis=x;
  	ax.y_axis=y;
	ax.type="ForeGround";
	ForeGround.push_back(ax);
  }
	else
	{
	ax.x_axis=x;
        ax.y_axis=y;
	ax.type="Background";
	Background.push_back(ax);
  }

}


int main(int argc, char **argv) {


    if (argc != 4) {
        cout << "Usage: ../seg input_image initialization_file output_mask" << endl;
        return -1;
    }

    // Load the input image
    Mat in_image;
    in_image = imread(argv[1]/*, CV_LOAD_IMAGE_COLOR*/);

    if (!in_image.data) {
        cout << "Could not load input image!!!" << endl;
        return -1;
    }

    if (in_image.channels() != 3) {
        cout << "Image does not have 3 channels!!! " << in_image.depth() << endl;
        return -1;
    }

    // the output image
    Mat out_image = in_image.clone();

    ifstream f(argv[2]);
    if (!f) {
        cout << "Could not load initial mask file!!!" << endl;
        return -1;
    }

    int width = in_image.cols;
    int height = in_image.rows;	
    vector < int > src;
    vector < int > des;
    int Pixel_count = (height * width)+2;	    

    //reading config file
    int n;
    f>>n;	
    int source =0;
    int sink =Pixel_count-1;
    vector< vector<pair<int,int> > > adj(Pixel_count);
    Mat output(in_image.rows, in_image.cols, in_image.type());
   //setting image markers
    vector<axis> ForeGround;
    vector<axis> Background;

   // get the initil pixels
    for(int i=0;i<n;++i){
        int x, y, t;
        f>>x>>y>>t;

        if(x<0 || x>=width || y<0 || y>=height){
            cout<<"Invalid pixel mask!"<<endl;
            return -1;
        }

        getInput(x,y,ForeGround,Background,t);
        
    }

    //Convert Image to Gray
    int ddepth = CV_16S;
    Mat Blur, gray;
    Mat srcc, src_gray;
    srcc = in_image.clone();
    cvtColor(srcc, src_gray, CV_BGR2GRAY);
    cvtColor(in_image, gray, CV_BGR2GRAY);
    Mat _x, _y;
    Mat abs__x, abs__y;
    Sobel(src_gray, _x, ddepth, 1, 0, 3, 1, 0, BORDER_DEFAULT);
    Sobel(src_gray, _y, ddepth, 0, 1, 3, 1, 0, BORDER_DEFAULT);
    convertScaleAbs(_x, abs__x);
    convertScaleAbs(_y, abs__y);
    addWeighted(abs__x, 0.5, abs__y, 0.5, 0, Blur);
    ::Marker = true;	

    
    //SetAndUpdateWeights
    UpdateWeights(adj, height, width, gray);
    setForeGround(src,ForeGround,width,adj,Pixel_count);
    setBackground(des,Background,width,adj,Pixel_count);
    
    //print_vector(adj);
    Compute(adj, source, sink, Pixel_count, output);
    out_image = output.clone();
    
    // write it on disk
    imwrite(argv[3], out_image);
    namedWindow( "Original image", WINDOW_AUTOSIZE );
    namedWindow( "Show Marked Pixels", WINDOW_AUTOSIZE );
    imshow( "Original image", in_image );
    imshow( "Show Marked Pixels", out_image );
    waitKey(0);
    return 0;
}
