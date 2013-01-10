#include <vector>
#include <math.h>
#include <string>
#include <map>
#include "usefulParsing.h"
#include "FormulaUser.h"
using namespace std;

#ifndef FORMULA_H
#define FORMULA_H

#define MAX_FLOATS 200

/* A is top, B is under, etc.
ADD  // B + A (also functions as an 'OR' if TRU is later called)
SUB  // B - A
MUL  // B * A (also functions as an 'AND')
DIV  // B / A
MOD  // B % A (*)
POW  // B ^ A
SWP  // swaps A and B
SIN  // sin(A) (**) (+)
MAX  // maximum of A and B
MIN  // minimum of A and B
NOT  // 1 if a is 0, 0 otherwise (**)
IFE  // 1 if B = A, 0 if B != A
IFG  // 1 if B > A, 0 if B <= A
TRU  // if A != 0, 1

SLF  // the previous value of the variable the formula is calculated for           (***)
TIM  // the FormulaUser's special variable, which is time in the case of this game (***)
EEE  // the value e  (***) (+)
PIE  // the value pi (***) (+)

*   = rounds a and b to integers
**  = only pops one value from the stack
*** = does not pop any values from the stack
+   = only works in runFloat

*/
enum FOpr{O_ADD, O_SUB, O_MUL, O_DIV, O_MOD, O_POW, O_SWP, O_SIN, O_MAX, O_MIN, O_NOT, O_IFE, O_IFG, O_TRU, O_SLF, O_TIM, O_EEE, O_PIE};

const double e  = 2.718281828459045;
const double pi = 3.141592653589793;

/*
 THE FORMULA CLASS
A formula stores a formula...
Example formulas can be seen in data/formulas.txt
Basically, they are stored and executed using reverse polish notation.ion
It's not incredibally efficient because bitpacks and takes lost of casting and bitwise operations. I should change this.
*/
class Formula {
    public:
        /*"length" is not a strict length, but the closer it is the more efficient it will be when constructing.
         Note that lengthwise: variables count as 2, floats count as 4 and integers can be form 1 to 4 depending on the size. */
        Formula(int length);
        virtual ~Formula();

        /* Push an operator duh. */
        void pushOperator(FOpr op);

        /* Push an integer.*/
        void pushInt(int num);

        /* Pushes a float: No more than FLOAT_MAX floats can exist in formulas at all in the application (the same float can be used multiple times and only count as one). */
        void pushFloat(float num);

        /* Pushes a var. TODO: more explain on this */
        void pushVar(int target, int type, int index);

        /* "run" will run using only integer operations on everything. it does not support floaty things, like floats and SIN
           "run float" is more flexible but slower (obvs) */
        int run(FormulaUser* user, StatHolderIntef* statHolder, int prevVal);
        double runFloat(FormulaUser* user, StatHolderIntef*, double prevVal);

        int getLength();
    protected:
    private:
        vector<unsigned char> commands; // A list of the commands that this formula goes through. A single command may take up multiple chars in this list.
        static float floats[MAX_FLOATS];// The floats used across all formulas. There can only be a maximum of MAX_FLOATS, so it is suggested to refraim from use.
        static int numFloats;           // The number of floats currently used.
};

#endif // FORMULA_H
