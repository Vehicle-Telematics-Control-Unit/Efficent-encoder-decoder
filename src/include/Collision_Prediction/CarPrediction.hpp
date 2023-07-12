#include <Vehicle.hpp>
#include <vector>

#define RADIUS 1.7f
#define WARN_TIME 5

class CarPrediction
{
    /* data */
private:
    static bool collide;
    static Vehicle my_vehicle;
    static float time2Collide;

public:
    static std::vector<Vehicle> surrounding_vehicles;

public:
    CarPrediction(/* args */);
    ~CarPrediction();
    static bool isWarn();
};

CarPrediction::CarPrediction(/* args */)
{
}

CarPrediction::~CarPrediction()
{
}
