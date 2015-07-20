#include "color.h"

Color::Color() {
  itsAge = -1;
}

 // constructor of Cat,
Color::Color(int initialAge)
{
  itsAge = initialAge;
}

Color::~Color()                 // destructor, just an example
{
    //delete itsAge;
}

// GetAge, Public accessor function
// returns value of itsAge member
int Color::GetAge()
{
   return itsAge;
}

// Definition of SetAge, public
// accessor function

 void Color::SetAge(int age)
{
   // set member variable its age to
   // value passed in by parameter age
   itsAge = age;
}

// definition of Meow method
// returns: void
// parameters: None
// action: Prints "meow" to screen
void Color::Meow()
{
   printf("Meow. (I'm %i years old)\n", itsAge);
}