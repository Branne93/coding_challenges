#include <array>
#include <queue>
#include <vector>
#include <stdint.h>

struct Node {
  int visited{0}; //change to bool
  int parent{-1};
  int distance{-1};
};

int getArrayIndex(const int x, const int y, const int nMapWidth){
  return x + y * nMapWidth;
}

std::array<int, 4> getNeighbourIndices(const int currentIndex, const int nMapWidth, const int nMapHeight){
  int y = currentIndex / nMapWidth;
  int mapSize = nMapWidth * nMapHeight;

  int above = currentIndex - nMapWidth;
  above = above < 0 ? -1 : above;
  int below = currentIndex + nMapWidth;
  below = mapSize <= below ? -1 : below;
  int left  = currentIndex - 1;
  left = y != left / nMapWidth || left < 0 ? -1 : left;
  int right = currentIndex + 1;
  right = y != right / nMapWidth ? -1 : right;
  
  return {above, below, left, right};
}

int getDistance(const int from, const int to, const int nMapWidth){
  int x1 = from % nMapWidth;
  int y1 = from / nMapWidth;
  int x2 = to % nMapWidth;
  int y2 = to / nMapWidth;
  return abs(x2 - x1) + abs(y2 - y1);
}

int FindPath(const int nStartX, const int nStartY,
             const int nTargetX, const int nTargetY, 
             const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
             int* pOutBuffer, const int nOutBufferSize){
  //Setup
  Node nodeMap[nOutBufferSize];
  int startIndex = getArrayIndex(nStartX, nStartY, nMapWidth);
  int targetIndex = getArrayIndex(nTargetX, nTargetY, nMapWidth);
  auto pairCompare = [targetIndex, nMapWidth](std::pair<int, int> left, std::pair<int, int> right) {
    if (left.second == right.second)
      return getDistance(left.first, targetIndex, nMapWidth) > getDistance(right.first, targetIndex, nMapWidth);
    return left.second > right.second;
  };
  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, decltype(pairCompare)> unVisitedNodes(pairCompare);

  unVisitedNodes.push({startIndex, 0});
  nodeMap[startIndex].distance = 0;
  bool targetFound = false;
  int currentIndex;

  //Start pathfinding
  while(!unVisitedNodes.empty() && !targetFound){
    std::pair<int, int> pair = unVisitedNodes.top();
    unVisitedNodes.pop();
    currentIndex = pair.first;
    if (nodeMap[currentIndex].visited == 1)
      continue;
    nodeMap[currentIndex].visited = 1;
    std::array<int, 4> neighbours = getNeighbourIndices(currentIndex, nMapWidth, nMapHeight);

    
    int distanceToNext = nodeMap[currentIndex].distance + 1;
    int neighbourDistance;
    auto shoulNodeBeVisited = [distanceToNext, pMap] (int neighbourIndex, int neighbourDistance)
      { return neighbourIndex != -1 && (neighbourDistance == -1  || distanceToNext < neighbourDistance)  && pMap[neighbourIndex] == 1;};
    
    for (int neighbourIndex : neighbours){
      neighbourDistance = nodeMap[neighbourIndex].distance;
      if (shoulNodeBeVisited(neighbourIndex, neighbourDistance)){
        nodeMap[neighbourIndex].parent = currentIndex;
        nodeMap[neighbourIndex].distance =  distanceToNext;
        if (targetIndex == neighbourIndex)
          targetFound = true;
        else
          unVisitedNodes.push({neighbourIndex, distanceToNext + getDistance(neighbourIndex, targetIndex, nMapWidth)});
      }
    }
  }
  if (!targetFound)
    return -1;
  
  currentIndex = targetIndex;
  pOutBuffer += nodeMap[targetIndex].distance -1;
  while (currentIndex != startIndex){
    *pOutBuffer-- = currentIndex;
    currentIndex = nodeMap[currentIndex].parent;
  }
  return nodeMap[targetIndex].distance;  
}

//Tests:
void CheckResult(int* facit, int* outputMap, int length){
  printf("From A*: ");
  for (int i = 0; i < length; i++)
    printf("%d, ", outputMap[i]);
  printf("\n");
  printf("Facit:   ");
  for (int i = 0; i < length; i++)
    printf("%d, ", facit[i]);
  printf("\n");
  bool good_result = true;
  for (int i = 0; i < length; i++)
    if (facit[i] != outputMap[i]){
      good_result = false;
      break;
    }
  printf("RESULT IS: %s\n", good_result ? "GOOD!" : "BAD!");
}

int test1(){
  printf("TEST 1\n");
  const int sizeX = 7;
  const int sizeY = 7;
  const unsigned char map []  = {
    1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,
    1,1,1,1,1,1,1
  };
  int outputMap[50];
  FindPath(1, 3, 4, 3, map, sizeX, sizeY, outputMap, 50);
  std::array<int, 3> facit = {23, 24, 25}; //Utan start
  CheckResult(facit.data(), outputMap, facit.size());
  return 0;
}

int test2(){
  printf("TEST 2\n");
  const int sizeX = 8;
  const int sizeY = 6;
  const unsigned char map []  = {
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,1,0,0,0,1,
    1,1,1,1,0,1,1,1,
    1,1,1,1,0,1,1,1,
    1,1,1,1,0,1,1,1
  };
  int outputMap[50];
  FindPath(1, 2, 7, 5, map, sizeX, sizeY, outputMap, 50);
  std::array<int, 11> facit = {18, 19, 11, 12, 13, 14, 15, 23, 31, 39, 47}; //Utan A
  CheckResult(facit.data(), outputMap, facit.size());
  return 0;
}

int test3(){
  printf("TEST 3\n");
  const int sizeX = 5;
  const int sizeY = 6;
  const unsigned char map []  = {
    1,1,1,1,1,
    1,0,0,0,0,
    1,1,1,1,1,
    0,0,0,0,1,
    1,1,1,0,1,
    1,0,1,1,1
  };
  int outputMap[50];
  FindPath(4, 0, 0, 5, map, sizeX, sizeY, outputMap, 50);
  std::array<int, 19> facit = {3, 2, 1, 0, 5, 10, 11, 12, 13, 14, 19, 24, 29, 28, 27, 22, 21, 20, 25};
  CheckResult(facit.data(), outputMap, facit.size());
  return 0;
}

int test4(){
  printf("TEST 4\n");
  const int sizeX = 5;
  const int sizeY = 6;
  const unsigned char map []  = {
    1,1,1,1,1, 
    1,1,1,1,1, 
    1,1,1,1,1, 
    1,1,1,1,1, 
    1,1,1,1,1, 
    1,1,1,1,1  
  };
  int outputMap[50];
  FindPath(4, 5, 0, 0, map, sizeX, sizeY, outputMap, 50);
  std::array<int, 9> facit = {24, 19, 14, 9, 4, 3, 2, 1, 0}; //Utan start
  CheckResult(facit.data(), outputMap, facit.size());
  return 0;
}

int test5(){
  printf("TEST 5\n");
  const int sizeX = 5;
  const int sizeY = 6;
  const unsigned char map []  = {
    1,1,1,1,1,
    1,1,0,1,1,
    1,0,1,1,1,
    0,1,1,0,0,
    1,1,0,1,1,
    1,1,1,1,1
  };
  int outputMap[50];
  FindPath(4, 5, 0, 0, map, sizeX, sizeY, outputMap, 50);
  std::array<int, 13> facit = {28, 27, 26, 21, 16, 17, 12, 13, 8, 3, 2, 1, 0}; //Utan A
  CheckResult(facit.data(), outputMap, facit.size());
  return 0;
}

int main(){
  test1();
  test2();
  test3();
  test4();
  test5();
  return 0;
}

