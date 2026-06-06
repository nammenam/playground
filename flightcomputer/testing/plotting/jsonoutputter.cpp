
#include <fstream>
#include <iostream>
#include "json.hpp" // Include the JSON library
                         //




#define STATE_VECTOR_SIZE 8 // 4 for quaternion, 3 for velocity, 1 for altitude
#define MEASURE_SIZE 4      // 3 for magnetometer, 1 for barometer


typedef struct
{
    float orientation_a;
    float orientation_b;
    float orientation_c;
    float orientation_d;
    float velocity_x;
    float velocity_y;
    float velocity_z;
    float altitude;
} StateVector;


typedef struct 
{
    StateVector state; // existing state vector (orientation, velocity, altitude)
    float P[STATE_VECTOR_SIZE][STATE_VECTOR_SIZE]; // State covariance matrix
    float Q[STATE_VECTOR_SIZE][STATE_VECTOR_SIZE]; // Process noise covariance matrix
    float F[STATE_VECTOR_SIZE][STATE_VECTOR_SIZE]; // Jacobian of the state transition function  f()
    float H[MEASURE_SIZE][STATE_VECTOR_SIZE];      // Jacobian of the measurement function       h()
    float R[MEASURE_SIZE][MEASURE_SIZE];           // Measurement noise covariance matri
    float K[STATE_VECTOR_SIZE][MEASURE_SIZE];      // Kalman gain
    float S[MEASURE_SIZE][MEASURE_SIZE];           // Innovation covariance
} KalmanFilter_t;


using json = nlohmann::json;

// Helper function to convert matrices to JSON arrays
template <size_t rows, size_t cols>
json matrixToJson(const float (&matrix)[rows][cols]) 
{
    json jMatrix = json::array();
    for (size_t i = 0; i < rows; ++i) 
    {
        json jRow = json::array();
        for (size_t j = 0; j < cols; ++j) 
        {
            jRow.push_back(matrix[i][j]);
        }
        jMatrix.push_back(jRow);
    }
    return jMatrix;
}

int main(int argc, char* argv[]) {

    std::string filePath; 

    if (argc > 1)
    {
        filePath = argv[1];
    }
    else
    {
        std::cout << "Please enter filepath" << std::endl;
        return -1;
    }


    std::ifstream binaryFile(filePath, std::ios::binary);
    std::ofstream jsonFile("kalman_filter_data.json");

    if (!binaryFile.is_open()) 
    {
        std::cout << "Could not open binary file." << std::endl;
        return 1;
    }

    if (!jsonFile.is_open()) 
    {
        std::cout << "Could not open JSON file." << std::endl;
        return 1;
    }

    json jArray = json::array();
    KalmanFilter_t kfData;

    int i = 0;

    while (binaryFile.peek() != EOF && i < 100)  //TODO better way to loop through the file
    {
        i++;
    
        // std::cout << kfData.state.orientation_a << std::endl;
        json jStateVector = {
            {"orientation_a", kfData.state.orientation_a},
            {"orientation_b", kfData.state.orientation_b},
            {"orientation_c", kfData.state.orientation_c},
            {"orientation_d", kfData.state.orientation_d},
            {"velocity_x", kfData.state.velocity_x},
            {"velocity_y", kfData.state.velocity_y},
            {"velocity_z", kfData.state.velocity_z},
            {"altitude", kfData.state.altitude}
        };

        json jData = {
            {"StateVector", jStateVector},
            {"P", matrixToJson(kfData.P)},
            {"Q", matrixToJson(kfData.Q)},
            {"F", matrixToJson(kfData.F)},
            {"H", matrixToJson(kfData.H)},
            {"R", matrixToJson(kfData.R)},
            {"K", matrixToJson(kfData.K)},
            {"S", matrixToJson(kfData.S)}
        };

        jArray.push_back(jData);
    }

    jsonFile << jArray.dump(4); // Pretty print with 4 spaces indent

    binaryFile.close();
    jsonFile.close();

    std::cout << "Data successfully written to kalman_filter_data.json." << std::endl;

    return 0;
}
