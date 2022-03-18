#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <cmath>
#include <stdexcept>

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
   
    vector<Position> PositionsOfInterest;

    vector<Position> OtherPositions;

public: 

    VehicleTracker() {}

    ~VehicleTracker() {}

    void ReadDataFile(string filename, float minLat, float maxLat, float minLng, float maxLng)
    {
    
       ifstream binaryFile;
       int size = 0;

       binaryFile.open(filename, ios::binary);
       if (!binaryFile.is_open())
       {
            cout << "Cannot open file "  << filename << endl;;
            throw runtime_error("Missing or invalid data file");
       }
       binaryFile.seekg(0, ios::end);
       size = static_cast<int>(binaryFile.tellg());
       binaryFile.seekg(0, ios::beg);
       size_t positionSize = sizeof(Position);
       while (binaryFile.tellg() < size)
       {
           Position position;
           binaryFile.read(reinterpret_cast<char*>(&position), positionSize);
           if (position.Latitude >= minLat && position.Latitude <= maxLat && position.Longitude >= minLng && position.Longitude <= maxLng)
           {
               PositionsOfInterest.push_back(position);
           }
           else 
           {
              OtherPositions.push_back(position);
           }
          
       }
       binaryFile.close();
    }

    vector<Vehicle> GetShortestDistance(vector<Position> &vehiclePositions)
    {
        const float maxDistance = 7600;
        vector<Vehicle> vehicles;
        for(Position val : vehiclePositions)
        {
            Position position;
            Vehicle vehicle { position, maxDistance };
            vehicles.push_back(vehicle);
        }

        vector<Position> SearchPool;
        if (PositionsOfInterest.size() > 0)
        {
            SearchPool = PositionsOfInterest;
        }
        else 
        {
            SearchPool = OtherPositions;
        }
        
        for(Position position: SearchPool) {
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

    float minLat = 180;
    float maxLat = -180;
    float minLng = 180;
    float maxLng = -180;
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

        if (position.Latitude < minLat)
        {
            minLat = position.Latitude;
        }
        if (position.Latitude > maxLat)
        {
            maxLat = position.Latitude;
        }

        if (position.Longitude < minLng)
        {
            minLng = position.Longitude;
        }
        if (position.Longitude > maxLng)
        {
            maxLng = position.Longitude;
        }
    }

    minLat -= 5;
    maxLat += 5;
    minLng -= 5;
    maxLng += 5;

    string filename = "VehiclePositions.dat";
    VehicleTracker* tracker = new VehicleTracker();
    
    auto startTime = high_resolution_clock::now();
    tracker->ReadDataFile(filename, minLat, maxLat, minLng, maxLng);
    auto endTime = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(endTime - startTime);
    printf("Data file read execution time: %d ms \n", duration);

    
    auto startT = high_resolution_clock::now();
    vector<Vehicle> vehicles = tracker->GetShortestDistance(positions);
    auto endT = high_resolution_clock::now();
    auto timeSpan = duration_cast<milliseconds>(endT - startT);
    printf("Closest position calculation execution time: %d ms \n", timeSpan);

    printf("\tCoordinate \t \t \t Nearest Position \n");
    for (int i = 0; i < vehicles.size(); i++)
    {
       printf("%d. (%f, %f) \t (%f, %f)  %f Km \n", i+1, positions[i].Latitude, positions[i].Longitude, vehicles[i].Position.Latitude, vehicles[i].Position.Longitude,vehicles[i].Distance);
    } 
    printf("Total execution time: %d ms \n", (duration+ timeSpan));
    system("pause");
}