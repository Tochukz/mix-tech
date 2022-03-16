#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>

using namespace std;
struct Position 
{
    int32_t PositionId;
    char VehicleRegistration[12];
    float Latitude;
    float Longitude;
    int64_t RecordedTimeUTC;
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

    VehicleTracker() {}

    ~VehicleTracker() {}

    void ReadDataFile(string  filename)
    {
       
       
       ifstream binaryFile;
       int size = 0;
       binaryFile.open(filename, ios::binary);
       binaryFile.seekg(0, ios::end);
       size = static_cast<int>(binaryFile.tellg());
       binaryFile.seekg(0, ios::beg);
 
       while (binaryFile.tellg() < size)
       {
           Position position;
           binaryFile.read(reinterpret_cast<char*>(&position.PositionId), sizeof(position.PositionId));
           binaryFile.read(reinterpret_cast<char*>(&position.VehicleRegistration), sizeof(position.VehicleRegistration));
           binaryFile.read(reinterpret_cast<char*>(&position.Latitude), sizeof(position.Latitude));
           binaryFile.read(reinterpret_cast<char*>(&position.Longitude), sizeof(position.Longitude));
           binaryFile.read(reinterpret_cast<char*>(&position.RecordedTimeUTC), sizeof(position.RecordedTimeUTC));
           
           //cout << position.PositionId << "\t" << position.VehicleRegistration << "\t" << position.Latitude << "\t" << position.Longitude << "\t" << position.RecordedTimeUTC << endl;
           Positions.push_back(position);
       }
       
       printf("Total Record %d\n", Positions.size());
       binaryFile.close();
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
    printf("Datafile read execusion time: %d ms \n", duration);
}