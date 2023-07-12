#include "../payloads.hpp"
#include <vector>

#define RADIUS 1.7f
#define WARN_TIME 5

class CarPrediction
{
    /* data */
private:
    bool collide;
    full_payload my, his;
    float time2Collide;

public:
    CarPrediction(/* args */);
    ~CarPrediction();
    bool isWarn();
};

CarPrediction::CarPrediction(/* args */)
{
}

CarPrediction::~CarPrediction()
{
}
