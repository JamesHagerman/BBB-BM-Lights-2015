#include <stdio.h>

class Color                   // begin declaration of the class
{
  public:                    // begin public section
    Color();
    Color(int initialAge);     // constructor
    ~Color();                  // destructor
    int GetAge();            // accessor function
    void SetAge(int age);    // accessor function
    void Meow();
 private:                   // begin private section
    int itsAge;              // member variable
};