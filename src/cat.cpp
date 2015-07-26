#include "cat.h"

Cat::Cat() {
  itsAge = -1;
}

 // constructor of Cat,
Cat::Cat(int initialAge)
{
  itsAge = initialAge;
}

Cat::~Cat()                 // destructor, just an example
{
    //delete itsAge;
}

// GetAge, Public accessor function
// returns value of itsAge member
int Cat::GetAge()
{
   return itsAge;
}

// Definition of SetAge, public
// accessor function

 void Cat::SetAge(int age)
{
   // set member variable its age to
   // value passed in by parameter age
   itsAge = age;
}

// definition of Meow method
// returns: void
// parameters: None
// action: Prints "meow" to screen
void Cat::Meow()
{
   printf("Meow. (I'm %i years old)\n", itsAge);
}