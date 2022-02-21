#include "Ant.h"
#include "MotionController.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " (output file name) " << std::endl;
    }

    MotionController control(10, 10, 10, 400, 500, argv[1]);
    control.AddObject(Object::Type::Nest, 20, 20, 10);
    control.AddObject(Object::Type::FoodSource, 80, 80, 10);
    control.AddObject(Object::Type::Obstacle, 45, 40, 5);
    control.Run();

    return 0;
}
