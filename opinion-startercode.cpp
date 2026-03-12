#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

/********************DO NOT EDIT**********************/
// Function prototype. Defined later.
void read_opinions(string filename); // reads file into opinions vector and updates total_nodes as needed
void read_edges(string filename); // reads file into edge_list, defined later
void build_adj_matrix(); // convert edge_list to adjacency matrix

int total_nodes = 0; // We keep track of the total number of nodes based on largest node id.


/****************************************************************/

/******** Create adjacency matrix and vector of opinions */
// simple vector to hold each node's opinion (0 or 1)
std::vector<int> opinions;

// global adjacency matrix initialized later
std::vector<std::vector<int>> adj;

// edge list: each row contains {source, target}
std::vector<std::vector<int>> edge_list;

void build_adj_matrix()
{
    // initialize adjacency matrix to total_nodes x total_nodes filled with 0
    adj.assign(total_nodes, std::vector<int>(total_nodes, 0));
    // populate adjacency from edge_list (treat as undirected)
    for(const auto &e : edge_list)
    {
        if(e.size() < 2) continue;
        int s = e[0];
        int t = e[1];   
        if(s >= 0 && s < total_nodes && t >= 0 && t < total_nodes)
        {
            adj[s][t] = 1;
            adj[t][s] = 1;
        }
    }

// ensure opinions vector has an entry for every node (default 0)
    if((int)opinions.size() < total_nodes)
        opinions.resize(total_nodes, 0);
}

double calculate_fraction_of_ones()
{
    if(total_nodes == 0) return 0.0;
    // ensure opinions vector sized correctly
    if((int)opinions.size() < total_nodes)
        opinions.resize(total_nodes, 0);

    int count_ones = 0;
    for(int i = 0; i < total_nodes; ++i)
        if(opinions[i] == 1) ++count_ones;

    return static_cast<double>(count_ones) / static_cast<double>(total_nodes);
}

// For a given node, count majority opinion among its neighbours. Tie -> 0.
int get_majority_friend_opinions(int node)
{
    if(node < 0 || node >= total_nodes) return 0;
    int neighbour_count = 0;
    int ones_count = 0;
    for(int j = 0; j < total_nodes; ++j)
    {
        if(adj[node][j])
        {
            ++neighbour_count;
            if(opinions[j] == 1) ++ones_count;
        }
    }

if(neighbour_count == 0) return 0; // no neighbours -> tie -> 0

if(ones_count > neighbour_count - ones_count) return 1;
// tie or majority zeros -> 0
return 0;
}

// Calculate new opinions for all voters and return if anyone's opinion changed
bool update_opinions()
{
    // ensure opinions size
    if((int)opinions.size() < total_nodes)
        opinions.resize(total_nodes, 0);

    std::vector<int> new_opinions = opinions;
    bool changed = false;

    for(int i = 0; i < total_nodes; ++i)
    {
        int maj = get_majority_friend_opinions(i);
        new_opinions[i] = maj;
        if(new_opinions[i] != opinions[i]) changed = true;
    }

    opinions.swap(new_opinions);
    return changed;

}

int main() {
    // no preallocation; vectors grow on demand

    // Read input files
    read_opinions("opinions.txt"); 
    read_edges("edge_list.txt");

    // convert edge list into adjacency matrix once we know total_nodes
    build_adj_matrix();
    
    cout << "Total nodes: " << total_nodes << endl;
    
    // Run simulation
    int max_iterations = 30;
    int iteration = 0;
    bool opinions_changed = true;
    
    // Print initial state
    cout << "Iteration " << iteration << ": fraction of 1's = " 
         << calculate_fraction_of_ones() << endl;
    
    /// (6)  //////////////////////////////////////////////
    while(opinions_changed && iteration < max_iterations)
    {
        ++iteration;
        opinions_changed = update_opinions();
        cout << "Iteration " << iteration << ": fraction of 1's = "
             << calculate_fraction_of_ones() << endl;
    }

    ////////////////////////////////////////////////////////
    // Print final result
    double final_fraction = calculate_fraction_of_ones();
    cout << "Iteration " << iteration << ": fraction of 1's = " 
         << final_fraction << endl;
    
    if(final_fraction == 1.0)
        cout << "Consensus reached: all 1's" << endl;
    else if(final_fraction == 0.0)
        cout << "Consensus reached: all 0's" << endl;
    else
        cout << "No consensus reached after " << iteration << " iterations" << endl;
    
    return 0;
}


/*********** Functions to read files **************************/ 

// Read opinion vector from file.
void read_opinions(string filename)
{
    ifstream file(filename);
    int id, opinion;
    while(file >> id >> opinion)
    {
        opinions.push_back(opinion);
        if(id >= total_nodes) total_nodes = id+1;
    }
    file.close();
}

// Read edge list from file and update total nodes as needed.
void read_edges(string filename)
{
    ifstream file(filename);
    int source, target;
    
    while(file >> source >> target)
    {
        edge_list.push_back({source, target});
        if(source >= total_nodes) total_nodes = source+1;
        if(target >= total_nodes) total_nodes = target+1;
    }
    file.close();
}

/********************************************************************** */