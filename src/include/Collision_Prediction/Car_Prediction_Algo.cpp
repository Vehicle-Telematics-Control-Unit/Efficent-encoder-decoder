#include <Car_Prediction_Algo.hpp>
#include <cmath>
// #include <math.h>
#include <iostream>

#define PI

bool CarPrediction::isWarn()
{
    time2Collide = INFINITY;
    collide = false;

        if (std::abs(my.x - v.x) <= RADIUS * 2 && std::abs(my.z - v.z) <= RADIUS * 2)
        {
            time2Collide = 0;
            std::cout << "Collision :: touching each other" << std::endl;
            break;
        }
        else if (std::abs(std::round(v.angle) - std::round(my.angle)) == 180 || std::abs(std::round(v.angle) - std::round(my.angle)) == 0)
        {
            if ((std::round(my.angle) == 90 || std::round(my.angle) == 270) && (std::round(v.angle) == 90 || std::round(v.angle) == 270))
            {
                std::cout << "Collision :: Angle = 90 or 270" << std::endl;
                if (std::abs(my.z - v.z) > RADIUS * 2 || ((std::round(my.angle) == 270 && std::round(v.angle) == 90 && v.x > my.x) || (std::round(my.angle) == 90 && std::round(v.angle) == 270 && v.x < my.x) || (my.speed == 0 && v.speed == 0) || (std::round(my.angle) == std::round(v.angle) && my.speed == v.speed) || (std::round(my.angle) == std::round(v.angle) && my.speed > v.speed && my.x > v.x) || (std::round(my.angle) == std::round(v.angle) && my.speed < v.speed && my.x < v.x) && std::abs(my.x - v.x) > RADIUS * 2))
                {
                    std::cout << "Collision :: Angle = 90 or 270 but no collision" << std::endl;
                    continue;
                }
            }
            else
            {
                std::cout << "Collision :: Angle != 90 nor 270" << std::endl;
                float m = std::tan(M_PI / 180 * std::round(my.angle));
                float c = my.x - m * my.z;
                float d = std::abs((m * v.z - v.x + c) / std::sqrt(m * m + 1));
                std::cout << "Collision :: " << (((std::round(my.angle) >= 0 && std::round(my.angle) < 90 && std::round(v.angle) > 180 && std::round(v.angle) < 270) || (std::round(my.angle) > 90 && std::round(my.angle) <= 180 && std::round(v.angle) > 270 && std::round(v.angle) < 360)) && v.z < my.z) << std::endl;
                // std::cout << "Collision :: D=" << d << std::endl;
                if (d > RADIUS * 2 || (((std::round(my.angle) > 180 && std::round(my.angle) < 270 && std::round(v.angle) >= 0 && std::round(v.angle) < 90) || (std::round(my.angle) > 90 && std::round(my.angle) <= 180 && std::round(v.angle) > 270 && std::round(v.angle) < 360)) && v.z > my.z) || (((std::round(my.angle) >= 0 && std::round(my.angle) < 90 && std::round(v.angle) > 180 && std::round(v.angle) < 270) || (std::round(my.angle) > 270 && std::round(my.angle) < 360 && std::round(v.angle) > 90 && std::round(v.angle) <= 180)) && v.z < my.z) || (my.speed == 0 && v.speed == 0) || (((my.speed == v.speed || (((std::round(my.angle) >= 0 && std::round(my.angle) < 90) || (std::round(my.angle) > 270 && std::round(my.angle) < 360)) && my.speed > v.speed && my.z > v.z) || (((std::round(my.angle) >= 0 && std::round(my.angle) < 90) || (std::round(my.angle) > 270 && std::round(my.angle) < 360)) && my.speed < v.speed && my.z < v.z) || (((std::round(my.angle) > 180 && std::round(my.angle) < 270) || (std::round(my.angle) > 90 && std::round(my.angle) <= 180)) && my.speed > v.speed && my.z < v.z) || (((std::round(my.angle) > 180 && std::round(my.angle) < 270) || (std::round(my.angle) > 90 && std::round(my.angle) <= 180)) && my.speed < v.speed && my.z > v.z))) && std::round(my.angle) == std::round(v.angle)))
                {
                    std::cout << "Collision :: Angle != 90 nor 270 and no collision" << std::endl;
                    continue;
                }
            }
            if (std::round(my.angle) == std::round(v.angle))
            {
                time2Collide = std::abs(std::sqrt(std::pow(my.x - v.x, 2) + std::pow(my.z - v.z, 2)) - RADIUS * 2) / std::abs(my.speed - v.speed);
                std::cout << "Collision :: Calculating AngleV = AngleM" << std::endl;
            }
            else
            {
                time2Collide = std::abs(std::sqrt(std::pow(my.x - v.x, 2) + std::pow(my.z - v.z, 2)) - RADIUS * 2) / (my.speed + v.speed);
                std::cout << "Collision :: Calculating AngleV != AngleM" << std::endl;
            }
        }
        else
        {
            if (!(my.speed == 0 && v.speed == 0))
            {
                if (my.speed == 0)
                {
                    // Debug.Log("Collision :: our vehicle is Stationary");
                    if (std::round(v.angle == 90) || std::round(v.angle == 270))
                    {
                        // Debug.Log("Collision :: surrounding vehicle angle is 90 || 270");
                        if (!(std::abs(my.z - v.z) > RADIUS * 2 || (my.x - v.x > 0 && std::round(v.angle == 270)) || (my.x - v.x < 0 && std::round(v.angle == 90))))
                        {
                            // Debug.Log("Collision :: potential collision");
                            time2Collide = (std::abs(my.x - v.x) - RADIUS * 2) / v.speed;
                        }
                    }
                    else
                    {
                        // Debug.Log("Collision :: surrounding vehicle angle is not 90 || 270");
                        float m = std::tan(std::round(v.angle * M_PI / 180.0));
                        float c = v.x - m * v.z;
                        float d = std::abs((m * my.z - my.x + c) / std::sqrt(m * m + 1));
                        if (!(d > RADIUS * 2 || (v.x - my.x >= 0 && v.z - my.z > 0 && v.angle >= 0 && v.angle < 90) || (v.x - my.x >= 0 && v.z - my.z < 0 && v.angle > 90 && v.angle <= 180) || (v.x - my.x < 0 && v.z - my.z < 0 && v.angle > 180 && v.angle < 270) || (v.x - my.x < 0 && v.z - my.z > 0 && v.angle > 270 && v.angle < 360)))
                        {
                            // Debug.Log("Collision :: potential collision");
                            time2Collide = (std::sqrt(std::pow(my.x - v.x, 2) + std::pow(my.z - v.z, 2)) - RADIUS * 2) / v.speed;
                        }
                    }
                }
                else if (v.speed == 0)
                {
                    // Debug.Log("Collision :: surrounding vehicle is Stationary");
                    if (std::round(my.angle) == 90 || std::round(my.angle) == 270)
                    {
                        // Debug.Log("Collision :: our vehicle angle is 90 || 270");
                        if (!(std::abs(my.z - v.z) > RADIUS * 2 || (v.x - my.x > 0 && std::round(my.angle) == 270) || (v.x - my.x < 0 && std::round(my.angle) == 90)))
                        {
                            // Debug.Log("Collision :: potential collision");
                            time2Collide = (std::abs(my.x - v.x) - RADIUS * 2) / my.speed;
                        }
                    }
                    else
                    {
                        // Debug.Log("Collision :: our vehicle angle is not 90 || 270");
                        float m = std::tan(std::round(my.angle * M_PI / 180.0));
                        float c = my.x - m * my.z;
                        float d = std::abs((m * v.z - v.x + c) / std::sqrt(m * m + 1));
                        if (!(d > RADIUS * 2 || (my.x - v.x >= 0 && my.z - v.z > 0 && my.angle >= 0 && my.angle < 90) || (my.x - v.x >= 0 && my.z - v.z < 0 && my.angle > 90 && my.angle <= 180) || (my.x - v.x < 0 && my.z - v.z < 0 && my.angle > 180 && my.angle < 270) || (my.x - v.x < 0 && my.z - v.z > 0 && my.angle > 270 && my.angle < 360)))
                        {
                            // Debug.Log("Collision :: potential collision");
                            time2Collide = (std::sqrt(std::pow(my.x - v.x, 2) + std::pow(my.z - v.z, 2)) - RADIUS * 2) / my.speed;
                        }
                    }
                }
                else
                {
                    // AI will predict
                }
            }
        }

}