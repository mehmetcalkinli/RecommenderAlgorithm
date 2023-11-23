
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include<vector>
#include<algorithm>
#include <unordered_map>
#include <cmath>
#include <map>


struct User {
    int item;
    double rating;
    double sum;
};
struct Movie {
    int userId;
    double rating;
    double sum;
};
struct SumOfRatings
{
    int id;
    double sum = 0;
};
struct Sim
{
    int secondUserId;
    double similarity;
};
struct User1
{
    int user1Id;
    double rating;
};
struct User2
{
    int user2Id;
    double rating;
};
struct Comp
{
    int itemId;
    User1 user1;
    User2 user2;
};
struct strItem
{
    int fileId;
    int itemId;
};
struct Item {
    int itemId;
};
struct Predicted
{
    int itemId;
    double predicted;
};
struct reverseTestMap
{
    int userId;
    int itemId;
    double rating;
};

bool operator==(const Item& lhs, const Item& rhs) {
    return lhs.itemId == rhs.itemId;
}



std::unordered_map<int, std::vector<std::vector<Comp>>> compMap;
std::unordered_map<int, std::vector<Sim>> similarityMap;
std::unordered_map<int, std::vector<Sim>>* similarityMap_ptr = &similarityMap;
std::unordered_map<int, std::vector<Predicted>>predictedMap;
std::map<int, reverseTestMap>submissionMap;
std::map<int, User> common_elements;
std::map<int, User>* common_elements_ptr=&common_elements;

std::vector<Sim> sameItem;


void cosineSimilarity(std::unordered_map<int, std::map<int, User>>& map, int user1Id, int user2Id) {
    double dotProduct = 0;
    double magnitude1 = 0;
    double magnitude2 = 0;
    double similarity = 0;
    common_elements.clear();
    int index = 0; 

   
    std::set_intersection(map[user1Id].begin(), map[user1Id].end(), map[user2Id].begin(), map[user2Id].end(),
        std::inserter(*common_elements_ptr, common_elements.begin()),
        [&](const auto& lhs, const auto& rhs)
        {return lhs.first < rhs.first;});  

    for (const auto& v : *common_elements_ptr)
    {        
        int index = v.first;
        if (index!=0)
        {
            dotProduct += map[user1Id][index].rating * map[user2Id][index].rating;
            magnitude1 += map[user1Id][index].rating * map[user1Id][index].rating;
            magnitude2 += map[user2Id][index].rating * map[user2Id][index].rating;
        }        
    }
    magnitude1 = sqrt(magnitude1);
    magnitude2 = sqrt(magnitude2);
        
    similarity= dotProduct / (magnitude1 * magnitude2);

    similarityMap[user1Id].push_back({ user2Id, similarity });
}

void predictRating(int userId, int itemId, std::unordered_map<int, std::map<int, User>>& userMap, std::unordered_map<int, std::vector<Sim>>& similarityMap, std::unordered_map<int, std::unordered_map<int,strItem>> &testMap)
{   
    double predict;
    double sim;
    int actualId;
    int file_id;
    int count = 0;
    double sumOfPredicts = 0;
    sameItem.clear();
    int elementCount = 0;

    
    for (auto it1 = similarityMap[userId].begin(); it1 != similarityMap[userId].end(); it1++)
    {
        sim = it1->similarity;
        actualId = it1->secondUserId;

        if (userMap[actualId].count(itemId) == 1 && sim!=1)
        {
            sameItem.push_back({ actualId,sim });
        }
    }

    std::sort(sameItem.begin(), sameItem.end(), [](const Sim& a, const Sim& b)
        {
            return a.similarity > b.similarity;
        });


    if (sameItem.size()<20)
    {
        elementCount = sameItem.size();
    }
    else
    {
        elementCount = 20;
    }


    for (size_t i = 0; i < elementCount; i++)
    {
        sumOfPredicts += userMap[sameItem[i].secondUserId][itemId].rating;
        count++;
    }
   
   
   
   
    predict = sumOfPredicts / count;
    file_id = testMap[userId][itemId].fileId;

    submissionMap[file_id].userId = userId;
    submissionMap[file_id].itemId = itemId;
    submissionMap[file_id].rating=predict;
}

int main()
{
    

    std::fstream inputFile;
    std::fstream testFile;
    std::ofstream outputFile;

    std::string userId;
    std::string itemId;
    std::string rating;
    std::string testId;
    std::string testUserId;
    std::string testItemId;

    int nUserId = 0;
    int nItemId = 0;
    double dRating = 0;
    int nTestIndex;
    bool isFirstLine = 1;
    bool isTestFirstLine = 1;
    double sum = 0;

    
    std::unordered_map<int, std::map<int, User>> userMap;
    std::unordered_map<int, std::map<int, User>>* userMap_ptr = &userMap;
    std::unordered_map<int, std::vector<User>> unordered_userMap;
    std::unordered_map<int, std::vector<Movie>> movieMap;
    std::vector<SumOfRatings> userSum;
    std::vector<SumOfRatings> movieSum;
    std::unordered_map<int, std::vector<strItem>> itemMap;
    std::unordered_map<int, std::unordered_map<int, strItem>> testMap;
    std::unordered_map<int, std::unordered_map<int, strItem>>*testMap_ptr=&testMap;
    

    inputFile.open("train.csv", std::ios::in);
    if (!inputFile.is_open()) { std::cout << "ERROR"; }
    if (inputFile.is_open())
    {

        while (!inputFile.eof())
        {
            getline(inputFile, userId, ',');
            getline(inputFile, itemId, ',');
            getline(inputFile, rating, '\n');

            

            if (userId != "" && itemId != "" && rating != "")
            {
                bool found = 0;

                if (isFirstLine == 1)
                {
                    isFirstLine = 0;
                }
                else
                {
                    
                    nUserId = std::stoi(userId);
                    nItemId = std::stoi(itemId);
                    dRating = std::stod(rating);
                    
                    unordered_userMap[nUserId].push_back({ nItemId,dRating,sum });

                    userMap[nUserId][nItemId].item = nItemId;
                    userMap[nUserId][nItemId].rating = dRating;
                    userMap[nUserId][nItemId].sum = 0;



                    movieMap[nItemId].push_back({ nUserId,dRating,sum });

                   
                    
                    
                }
            }

        }
    }
    inputFile.close();

    testFile.open("test.csv", std::ios::in);
    if (!testFile.is_open()) { std::cout << "ERROR"; }
    if (testFile.is_open()) 
    {
        while (!testFile.eof())
        {
            getline(testFile, testId, ',');
            getline(testFile, testUserId, ',');
            getline(testFile, testItemId, '\n');

            if (testId != "" && testUserId != "" && testItemId != "")
            {
                if (isTestFirstLine == 1)
                {
                    isTestFirstLine = 0;
                }
                else
                {
                    nTestIndex = std::stoi(testId);
                    nUserId = std::stoi(testUserId);
                    nItemId = std::stoi(testItemId);

                   
                    testMap[nUserId][nItemId].fileId= nTestIndex;
                    testMap[nUserId][nItemId].itemId = nItemId;
                }
            }
           
        }
    }
    testFile.close();

   for (const auto& m : unordered_userMap)
   {
       std::vector<User> x;
       x = m.second;

       sum = 0;

       for (size_t i = 0; i < x.size(); i++)
       {
           sum += x[i].rating;
       }
       userMap[m.first][0].sum = sum;
       


       userSum.push_back({ m.first,sum });


   }
   for (const auto& m : movieMap)
   {
       std::vector <Movie> x;
       x = m.second;

       sum = 0;

       for (size_t i = 0; i < x.size(); i++)
       {
           sum += x[i].rating;
       }
       movieMap[m.first][0].sum = sum;



       movieSum.push_back({ m.first,sum });


   }

for (size_t i = 0; i < 10; i++)
{
    std::sort(userSum.begin(), userSum.end(), [](const SumOfRatings& a, const SumOfRatings& b)
        {
        return a.sum > b.sum;
    });

    std::sort(movieSum.begin(), movieSum.end(), [](const SumOfRatings& a, const SumOfRatings& b)
        {
            return a.sum > b.sum;
        });

        i++;
}

             std::cout << "Top 10 users with the biggest total ratings:" << std::endl;
             for (size_t i = 0; i < 10; ++i)
             {
                 std::cout <<"User " << userSum[i].id << "      " << userSum[i].sum << std::endl;
             }

             std::cout << "Top 10 movies with the biggest total ratings:" << std::endl;
             for (size_t i = 0; i < 10; ++i)
             {
                 std::cout << "Movie " << movieSum[i].id << "      " << movieSum[i].sum << std::endl;
             }







for (auto it1 = testMap.begin(); it1 != testMap.end(); it1++)
{
    auto& id1 = it1->first;



    for (auto it2 = userMap.begin(); it2 != userMap.end(); it2++)
    {
        auto& id2 = it2->first;

        if (id1 != id2)
        {
            cosineSimilarity(*userMap_ptr, id1, id2);
        }
    }

    for (auto it3 = testMap[id1].begin(); it3 != testMap[id1].end(); it3++)
    {
        auto& second = it3->second;

        predictRating(id1, second.itemId, *userMap_ptr, *similarityMap_ptr, *testMap_ptr);

    }
}




bool firstOutput = 1;
outputFile.open("submission.csv", std::ios::out);

for (auto it1 = submissionMap.begin(); it1 != submissionMap.end(); it1++)
{
    auto& file_id = it1->first;
   reverseTestMap vec = it1->second;
   if (firstOutput == 1)
   {
       outputFile << "ID,Predicted" << "\n";
   }
   outputFile << file_id << "," << vec.rating << "\n";
   firstOutput = 0;
}
outputFile.close();









}

 