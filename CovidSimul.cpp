#include <iostream>
#include <set>
#include <queue>
#include <random>
#include <vector>
#include <cstdlib>

#define NUM 100
using namespace std;

// Defining Nodes
struct node
{
  int node_id;
  int timestamp; // Eventually stores the recovery time of this node
  string event_type;
  int timestamp_infection; // Infection time of this node
  int min_distance;        // Stores the shortest distance of this node from a particular node

  bool operator<(const node &t) const
  {
    return (this->node_id < t.node_id);
  }
} persons[NUM];

/*Simulating Coin toss*/
int coin_toss()
{
  // Since tossing of coin is uniform distribution we use the below code to get 0 or 1
  random_device dev;
  mt19937 rng(dev());
  uniform_int_distribution<mt19937::result_type> dist6(0, 1); // distribution in range [1, 6]

  return dist6(rng);
}

/*Generates a number between 0 and 5*/
int num_generator()
{
  // Same code just change the parameters
  random_device dev;
  mt19937 rng(dev());
  uniform_int_distribution<mt19937::result_type> dist6(0, 5); // distribution in range [1, 6]

  return dist6(rng);
}

/*We store the graph using Adjaceny Matrix*/
int adjMat[NUM][NUM];

/*Function which generates a random graph*/
void graph_generate()
{

  /*Since the graph we are generating is an undirected graph
  we can just assign values to the upper triangular part of the adjacency matrix
  and the bottom triangular part will also have the same values*/
  for (int i = 0; i < NUM - 1; i++)
  {

    for (int j = i + 1; j < NUM; j++)

    {
      int toss = coin_toss();
      adjMat[i][j] = (toss == 1) ? 1 : 0;
      adjMat[j][i] = adjMat[i][j];
    }
  }

  for (int i = 0; i < NUM; i++)
  {
    adjMat[i][i] = 0; // No self links between persons

    persons[i].node_id = i;
    persons[i].timestamp = 0;
    persons[i].timestamp_infection = 0;
    persons[i].min_distance = 0;
  }
}

/*Writing an algorithm to find the shorest distance from the start node*/

void shortest_distance(int start) // Shortest Distance of all the vertices from the start node
{
  queue<int> nodes; // Queue Containg Nodes with node_id's
  nodes.push(start);
  while (!nodes.empty()) // Nodes is not empty
  {
    int temp = nodes.front();
    nodes.pop();
    for (int i = 0; i < NUM; i++)
    {
      /*We write condition to check whether there is a connection,
      if the connection already has a min_distance and it is not same as
      start.*/
      if ((adjMat[temp][i] == 1) && (persons[i].min_distance == 0) && (i != start))
      {
        persons[i].min_distance = persons[temp].min_distance + 1; // There is no connection between start and i.
        nodes.push(persons[i].node_id);
      }
    }
  }
}

/*Creating Sets for Susceptible,Infected and Recovered*/
set<int> S; // For Susceptible
set<int> I; // For Infected
set<int> R; // For Recovered

int main()
{
  graph_generate();

  /*First all the nodes will be in the Susceptible Set*/
  for (int i = 0; i < NUM; i++)
  {
    S.insert(i);
  }

  /*Creating the min-queue using lambda to compare*/

  auto cmp = [](node left, node right)
  { return (left.timestamp) >= (right.timestamp); };
  priority_queue<node, vector<node>, decltype(cmp)> Q(cmp);

  /*Now we write the simulation*/
  /*Choosing a random number*/
  srand(time(NULL));
  int random = rand() % NUM;

  // After choosing the start node we can assign the min_distances from this node
  shortest_distance(random);

  persons[random].event_type = "Infection";
  Q.push(persons[random]);

  /*Writing the while loop*/
  while (!Q.empty())
  {
    node e = Q.top(); // Min time_stamp Node
    Q.pop();
    if (e.event_type == "Recovery")
    {
      R.insert(e.node_id);
      I.erase(e.node_id);
    }
    else if (e.event_type == "Infection")
    {
      I.insert(e.node_id);
      S.erase(e.node_id);

      for (int i = 0; i < NUM; i++) // Loop for checking connection
      {
        if (adjMat[e.node_id][i] == 1) // Condition for checking if connection is present
        {
          if (S.count(persons[i].node_id) == 1) // Checking the person is susceptible or not
          {
            int num_of_tosses = 0;
            int head_present = 0;
            ;

            for (int j = 0; j < 5; j++)
            {
              int toss2 = coin_toss();
              if (toss2 == 1)
              {
                num_of_tosses++;
                head_present = 1;
                break; // If it is heads we break out of the loop
              }
              num_of_tosses++;
            }

            if (head_present == 1)
            {
              /*Adding the Infection Event into Q*/
              persons[i].timestamp = e.timestamp + num_of_tosses;
              persons[i].timestamp_infection = persons[i].timestamp;
              persons[i].event_type = "Infection";
              Q.push(persons[i]);

              /*Generating the Recovery event*/

              int k = e.timestamp + num_of_tosses + num_generator() + 1;

              /*Adding the Recovery Event into the Q*/
              persons[i].timestamp = k;
              persons[i].event_type = "Recovery";
            }
          }
        }
      }
    }
  }

  /*Code to check the final conditions of the persons*/
  // for(int i =0;i<NUM;i++)
  // {
  //     cout<<persons[i].node_id<<" "<<persons[i].timestamp<<" "<<persons[i].timestamp_infection<<" "<<persons[i].event_type<<" "<<persons[i].min_distance<<endl;
  //}

  /*Process for printing the table*/
  /*Note that at last there still will be one person infected who is
  the first guy that got infected because we did not add a recovery event for
  him in our code*/

  int max_time = 0; // To find the number of rows in table;
  for (int i = 0; i < NUM; i++)
  {
    if (persons[i].timestamp > max_time)
    {
      max_time = persons[i].timestamp;
    }
  }

  int infected_count[max_time + 1]; // It gives us how many people were infected on a particular time
  int recovered_count[max_time + 1];
  int susceptible_count[max_time + 1];

  for (int i = 0; i <= max_time; i++)
  {
    infected_count[i] = recovered_count[i] = susceptible_count[i] = 0;
  }

  for (int i = 0; i < NUM; i++)
  {
    // for(int j =0;j<=max_time;j++)
    // {
    //     if(persons[i].timestamp_infection == j)
    //     {
    //         infected_count[j]++;
    //     }

    //     if(persons[j].timestamp == j)
    //     {
    //         recovered_count[j]++;
    //     }
    // }
    infected_count[persons[i].timestamp_infection]++;
    recovered_count[persons[i].timestamp]++;
  }
  recovered_count[0]--;

  for (int i = 1; i <= max_time; i++)
  {
    // Cumulative Counting to find total number of recovered on a particular time
    recovered_count[i] = recovered_count[i] + recovered_count[i - 1];

    // Cumulative counting of infected to find total number of infected at a particular time
    infected_count[i] = infected_count[i] + infected_count[i - 1];
  }

  for (int i = 0; i <= max_time; i++)
  {
    /*To get people who are currently infected at a particular time, we need
    to subtract the total number of recovered at a particular time from total
    number of infected.*/
    infected_count[i] = infected_count[i] - recovered_count[i];

    /*Since total number of persons is 100 we can get susceptible amount easily*/
    susceptible_count[i] = NUM - infected_count[i] - recovered_count[i];
  }

  cout << "\nTABLE-1\n";
  cout << "Time \t\t Infected \t Recovered \t Succeptible " << endl;
  for (int i = 0; i <= max_time; i++)
  {
    cout << i << " \t \t " << infected_count[i] << " \t \t " << recovered_count[i] << " \t \t " << susceptible_count[i] << endl;
  }

  /*Printing the table of comparing the infection timestamps and shortest distance*/
  cout << "\n\nSHORTEST DISTANCE VS INFECTION TIMESTAMP"
       << "\n";
  cout << "Node id\t\tInfection_Time\tShortest_distance" << endl;

  for (int i = 0; i < NUM; i++)
  {
    cout << i << "\t\t" << persons[i].timestamp_infection << "\t\t" << persons[i].min_distance << endl;
  }

  return 0;
}