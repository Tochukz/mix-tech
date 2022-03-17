#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <cmath>

using namespace std;

struct Position 
{
    int32_t PositionId;
    char VehicleRegistration[12];
    float Latitude;
    float Longitude;
    int64_t RecordedTimeUTC;
};
struct Vehicle
{
    Position Position;
    double Distance;
};

struct VehicleTracker
{
   
private:
    int32_t PositionId;
    char VehicleRegistration[12];
    float Latitude;
    float Longitude;
    int64_t RecordedTimeUTC;
   
    vector<Position> Positions;

public: 

    VehicleTracker() {

    }

    ~VehicleTracker() {}

    void ReadDataFile(string  filename)
    {
    
       ifstream binaryFile;
       int size = 0;

       binaryFile.open(filename, ios::binary);
       binaryFile.seekg(0, ios::end);
       size = static_cast<int>(binaryFile.tellg());
       binaryFile.seekg(0, ios::beg);
       size_t positionSize = sizeof(Position);
       while (binaryFile.tellg() < size)
       {
           Position position;
           binaryFile.read(reinterpret_cast<char*>(&position), positionSize);
           Positions.push_back(position);
       }
       
       printf("Total Record %d\n", Positions.size());
       binaryFile.close();
    }

    vector<Vehicle> GetShortestDistance(vector<Position> &vehiclePositions)
    {
        const float maxDistance = 7600;
        vector<Vehicle> vehicles;
        int i = 0;
        for(Position val : vehiclePositions)
        {
            Position position;
            Vehicle vehicle { position, maxDistance };
            vehicles.push_back(vehicle);
            i++;
        }

        for(Position position: Positions) {
            for(size_t i = 0; i < vehiclePositions.size(); i++) {
               double distance = CalculateDistance(position, vehiclePositions[i]);
               if (distance < vehicles[i].Distance)
               {
                   Vehicle vehicle { position, distance };
                   vehicles[i] = vehicle;
               }            
            }
        }
        return vehicles;
    }

    double CalculateDistance(Position pointA, Position pointB)
    {
        const double earthRadius = 6378.8;
        const  double factor = 180 / (22/7);
        double latA = pointA.Latitude / factor;
        double latB = pointB.Latitude / factor;
        double lngA  = pointA.Longitude / factor;
        double lngB = pointB.Longitude / factor;

        double distance = earthRadius * acos(sin(latA) * sin(latB) + cos(latA) * cos(latB) * cos(lngB - lngA));
        return distance;
    }
};


int main()
{
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;

    string filename = "VehiclePositions.dat";
    VehicleTracker* tracker = new VehicleTracker();
    
    auto startTime = high_resolution_clock::now();
    tracker->ReadDataFile(filename);
    auto endTime = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(endTime - startTime);
    printf("Datafile read execution time: %d ms \n", duration);

    float coordinates[10][2] = {
        {34.544909, -102.100843},
        {32.345544, -99.123124},
        {33.234235, -100.214124},
        {35.195739, -95.348899},
        {31.895839, -97.789573},
        {32.895839, -101.789573},
        {34.115839, -100.225732},
        {32.335839, -99.992232},
        {33.535339, -94.792232},
        {32.234235, -100.222222}
    };
    vector<Position> positions;    
    for (auto point: coordinates)
    {   
        Position position = {1, "Position ", point[0], point[1]};
        positions.push_back(position);
    }

    auto startT = high_resolution_clock::now();
    vector<Vehicle> vehicles = tracker->GetShortestDistance(positions);
    auto endT = high_resolution_clock::now();
    auto timeSpan = duration_cast<milliseconds>(endT - startT);
    printf("Closest position calculation execution time: %d ms \n", timeSpan);

    printf("Shortest Distances: \n");
    for (int i = 0; i < vehicles.size(); i++)
    {
       printf("Lat: %f, Lng: %f => Lat: %f, Lng: %f, Dist: %f Km \n", positions[i].Latitude, positions[i].Longitude, vehicles[i].Position.Latitude, vehicles[i].Position.Longitude,vehicles[i].Distance);
    }

   /*
    auto startX = high_resolution_clock::now();

    printf("Total execution time: %d ms \n", (duration+ timeSpan));
    Position pointA { 1 , "Position 1",  53.32055555555556, -1.7297222222222221};
    Position pointB { 2, "Position 2", 53.31861111111111, -1.6997222222222223};
    double distance = tracker->CalculateDistance(pointA, pointB);
    printf("Distance = %f Km \n", distance);

    auto endX = high_resolution_clock::now();
    auto timeSpanX = duration_cast<milliseconds>(endX - startX);
    printf("Calculation execution time: %d ms \n", timeSpanX);
    */
}