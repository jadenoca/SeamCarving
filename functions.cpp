#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

// TODO Write this function
int energy(const Pixel *const*image, int col, int row, int width, int height)
{
  if(row >= height || col >=width || row < 0 || col < 0 || width < 1 || height < 1)
  {
    throw invalid_argument("Bad input!");
  }
  
  int xGradient = 0;
  int yGradient = 0;
  int diff;
  //Declaring the coordinate values to be used for all non-edge pieces
  int yUpper = row + 1;
  int yLower = row -1;
  int xUpper = col + 1;
  int xLower = col -1;
 
  if(width == 1 && height == 1)
  {
    return 0;
  }

  //Coordinate values should only be modified if they are an edge piece
  if(row == 0) //Border pixel on the top
  {
    if(height == 1)
    {
      yUpper = 0;
    } else{
      yUpper = 1;
    }
    yLower = height -1;
  } 
  else if(row == height -1) //Border pixel on the bottom
  {
    yUpper = 0;
    if(height == 1)
    {
      yLower = 0;
    }
  } 
  if(col == 0) //Border pixel on the left
  {
    if(width == 1){
      xUpper = 0;
    } else{
      xUpper = col + 1;
    }
    xLower = width -1;
  }
  else if(col == width -1) //Border pixel on the right
  {
    xUpper = 0;
    if(width == 1)
    {
      xLower = 0;
    } else{
      xLower = col -1;
    }
  } 
  diff = (image[xUpper][row].r - image[xLower][row].r);
  xGradient += diff * diff;
  diff = (image[xUpper][row].g - image[xLower][row].g);
  xGradient += diff * diff;
  diff = (image[xUpper][row].b - image[xLower][row].b);
  xGradient += diff * diff;

  diff = (image[col][yUpper].r - image[col][yLower].r);
  yGradient += diff * diff;
  diff = (image[col][yUpper].g - image[col][yLower].g);
  yGradient += diff * diff;
  diff = (image[col][yUpper].b - image[col][yLower].b);
  yGradient += diff * diff;
  
  return yGradient + xGradient;
}

// TODO Write this function
int getVerticalSeam(const Pixel *const*image, int start_col, int width, int height, int* seam)
{
  int energySum = energy(image, start_col, 0, width, height);
  int current = start_col;
  int left, mid, right;
  seam[0] = start_col;
  for(int i = 1; i < height; i++)
  {
    mid = energy(image, current, i, width, height);
    if(current > 0 && current < (width-1))
    {
     right = energy(image, current -1, i , width, height);
     left = energy(image, current + 1, i, width, height);
    } else if(current == 0)
    {
      right = 99999999;
      if(width == 1){
        left = 99999999;
      } else
      {
        left = energy(image, current + 1, i, width, height);
      }
    } else if(current == (width -1))
    {
      left = 99999999;
      if(width == 1){
        right = 99999999;
      } else
      {
        right = energy(image, current -1, i , width, height);
        }
    }

     if(right < left && right < mid)
     {    
       current--;
     }
     else if(left <= right && left < mid)
     {  
       current++;
     }
    
      seam[i] = current;    
      energySum += energy(image, current, i, width, height); 
  }
  
  return energySum;
}

// TODO Write this function
void removeVerticalSeam(Pixel **image, int& width, int height, int *verticalSeam)
{ 
  for(int i = 0; i < height; i++)
  {
    for(int j = verticalSeam[i]; j < width-1; j++)
    {
     image[j][i].r = image[j+1][i].r;
     image[j][i].g = image[j+1][i].g;
     image[j][i].b = image[j+1][i].b;
     }
  }
  width--;
}

// TODO Write this function for extra credit
int getHorizontalSeam(const Pixel *const*image, int start_row, int width, int height, int* seam)
{
  int a = image[0][0].r;
  int b = start_row + width + height + seam[0]; 
  return a + b;
}

// TODO Write this function for extra credit
void removeHorizontalSeam(Pixel **image, int width, int& height, int *horizontalSeam)
{
  int c = image[0][0].r;
  int d = width + height + horizontalSeam[0]; 
  horizontalSeam[0] = c + d;
}

int *findMinVerticalSeam(const Pixel *const*image, int width, int height)
{
  // initialize minSeam and minDistance to seam starting at first col (index 0)
  int *minSeam = new int[height]{0};
  int minDist = getVerticalSeam(image, 0, width, height, minSeam);

  int *candidateSeam = new int[height]{0};
  int candidateDistance = -1; // invalid distance

  // start at second col (index 1) since we initialized with first col (index 0)
  for (int col = 1; col < width; ++col)
  {
    candidateDistance = getVerticalSeam(image, col, width, height, candidateSeam);

    if (candidateDistance < minDist)
    { // new min
      //  swap min & candidate
      minDist = candidateDistance;
      int* temp = candidateSeam;
      candidateSeam = minSeam;
      minSeam = temp;
    }
  }

  // clean up 
  delete [] candidateSeam;

  return minSeam;
}

int *findMinHorizontalSeam(const Pixel *const*image, int width, int height)
{
  // initialize minSeam and minDistance to seam starting at first row (index 0)
  int *minSeam = new int[width]{0};
  int minDistance = getHorizontalSeam(image, 0, width, height, minSeam);

  int *candidateSeam = new int[width]{0};
  int candidateDistance = -1; // invalid distance

  // start at second row (index 1) since we initialized with first row (index 0)
  for (int row = 1; row < height; ++row)
  {
    candidateDistance = getHorizontalSeam(image, row, width, height, candidateSeam);

    if (candidateDistance < minDistance)
    { // new minimum
      //  swap min and candidate seams
      minDistance = candidateDistance;
      int* temp = candidateSeam;
      candidateSeam = minSeam;
      minSeam = temp;
    }
  }

    // clean up 
  delete [] candidateSeam;

  return minSeam;
}

Pixel **createImage(int width, int height)
{
  cout << "Start createImage... " << endl;

  // Create a one dimensional array on the heap of pointers to Pixels
  //    that has width elements (i.e. the number of columns)
  Pixel **image = new Pixel *[width] {}; // initializes to nullptr

  for (int col = 0; col < width; ++col)
  { // loop through each column
    // assign that column to a one dimensional array on the heap of Pixels
    //  that has height elements (i.e. the number of rows)
    try
    {
      image[col] = new Pixel[height];
    }
    catch (std::bad_alloc &e)
    {
      // clean up already allocated arrays
      for (int i = 0; i < col; ++i)
      {
        delete[] image[i];
      }
      delete[] image;
      // rethrow
      throw e;
    }
  }

  // initialize cells
  // cout << "Initializing cells..." << endl;
  for (int row = 0; row < height; ++row)
  {
    for (int col = 0; col < width; ++col)
    {
      // cout << "(" << col << ", " << row << ")" << endl;
      image[col][row] = {0, 0, 0};
    }
  }
  cout << "End createImage... " << endl;
  return image;
}

void deleteImage(Pixel **image, int width)
{
  cout << "Start deleteImage..." << endl;
  // avoid memory leak by deleting the array
  for (int i = 0; i < width; ++i)
  {
    delete[] image[i];
  }
  delete[] image;
  image = nullptr;
  cout << "End deleteImage..." << endl;
}

bool isValidColor(int colorVal)
{
  if (colorVal < 0 || colorVal > 255)
  {
    return false;
  }
  return true;
}

Pixel ** loadImage(string filename, int &width, int &height)
{
  cout << "Start loadImage..." << endl;
  // remove
  ifstream ifs(filename);
  if (!ifs.is_open())
  {
    throw std::invalid_argument("Failed to open input file (" + filename + ")");
  }

  string type;
  ifs >> type; // should be P3
  if (toupper(type.at(0)) != 'P' || type.at(1) != '3')
  {
    throw std::domain_error("Not PPM type P3 (" + type + ")");
  }
  ifs >> width;
  // cout << "w and h: " << w << " " << h << endl;
  if (ifs.fail())
  {
    throw std::domain_error("Read non-integer value for width");
  }
  if (width <= 0)
  {
    ostringstream oss;
    oss << "Width in file must be greater than 0 (" << width << ")";
    throw std::domain_error(oss.str());
  }

  ifs >> height;
  if (ifs.fail())
  {
    cout << "Read non-integer value for height" << endl;
  }
  if (height <= 0)
  {
    ostringstream oss;
    oss << "Height in file must be greater than 0 (" << height << ")";
    throw std::domain_error(oss.str());
  }

  int colorMax = 0;
  ifs >> colorMax;
  if (ifs.fail())
  {
    throw std::domain_error("Read non-integer value for max color value");
  }
  if (colorMax != 255)
  {
    ostringstream oss;
    oss << "Max color value must be 255 (" << colorMax << ")";
    throw std::domain_error(oss.str());
  }

  // load image throws exceptions but we will let them pass through
  Pixel **image = createImage(width, height);

  for (int row = 0; row < height; ++row)
  {
    for (int col = 0; col < width; ++col)
    {
      // cout << "Pixel(" << col << ", " << row << ")" << endl;
      ifs >> image[col][row].r;
      if (ifs.fail() && !ifs.eof())
      { // bad input that is not caused by being at the end of the file
        throw std::domain_error("Read non-integer value for red");
      }
      if (!isValidColor(image[col][row].r))
      {
        ostringstream oss;
        oss << "Invalid color value for red (" << image[col][row].r << ")";
        throw std::domain_error(oss.str());
      }

      ifs >> image[col][row].g;
      if (ifs.fail() && !ifs.eof())
      { // bad input that is not caused by being at the end of the file
        throw std::domain_error("Read non-integer value for green");
      }
      if (!isValidColor(image[col][row].r))
      {
        ostringstream oss;
        oss << "Invalid color value for green (" << image[col][row].r + ")";
        throw std::domain_error(oss.str());
      }

      ifs >> image[col][row].b;
      if (ifs.fail() && !ifs.eof())
      { // bad input that is not caused by being at the end of the file
        throw std::domain_error("Read non-integer value for blue");
      }
      if (!isValidColor(image[col][row].r))
      {
        ostringstream oss;
        oss << "Invalid color value for blue (" << image[col][row].r + ")";
        throw std::domain_error(oss.str());
      }
    }
  }
  cout << "End loadImage..." << endl;
  return image;
}

void outputImage(string filename, const Pixel *const *image, int width, int height)
{
  cout << "Start outputImage..." << endl;
  // remove code
  // declare/define and open output file stream with filename
  ofstream ofs(filename);
  // ensure file is open
  if (!ofs.is_open())
  {
    throw std::invalid_argument("Error: failed to open output file - " + filename);
  }
  ofs << "P3" << endl;
  ofs << width << " " << height << endl;
  ofs << 255 << endl;
  for (int row = 0; row < height; ++row)
  {
    for (int col = 0; col < width; ++col)
    {
      ofs << image[col][row].r << " ";
      ofs << image[col][row].g << " ";
      ofs << image[col][row].b << " ";
    }
    ofs << endl;
  }
  cout << "End outputImage..." << endl;
}