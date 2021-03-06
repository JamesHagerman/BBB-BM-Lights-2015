#include <stdio.h>

class Cat                   // begin declaration of the class
{
  public:                    // begin public section
    Cat();
    Cat(int initialAge);     // constructor
    ~Cat();                  // destructor
    int GetAge();            // accessor function
    void SetAge(int age);    // accessor function
    void Meow();
 private:                   // begin private section
    int itsAge;              // member variable
};