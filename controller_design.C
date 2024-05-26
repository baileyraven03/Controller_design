// This is a program desinged to enable the user to design a variant of a Pi/PID/PD controller
// The design will be based off of a vaiety of user information about the system and desired charicteristics of the controller.
// Ideally the user will use this with a singular transfer function to find the optimal controller type to use
// Written by Bailey Raven
// Date: 26/04/2024

#include <stdio.h>  // Including stdio.h header file
#include <math.h>   // Including math.h header file
#include <tgmath.h> // Including tgmath.h header file

void calcs(struct controller *c1, struct transfer_function t1, struct parameters *p1, int i);        // Declaring the calcs function to perform the bulk of the calculations
void controller_display(struct controller *c1, int i);                                               // Declaring the controller_display function to display the controller information
float userInputs(struct controller *c1, int i, struct transfer_function *t1, struct parameters *p1); // Declaring the userInputs function to get the user inputs for teh parameters and transfer function
void clear(void);                                                                                    // Declaring the clear function to clear the buffer

struct transfer_function // Defining the transfer function structure
{
    float a; // Defining the numeratior
    float b; // Defining the first s term denominator
    float c; // Defining the first pole term denominator
    float d; // Defining the second s term denominator
    float e; // Defining the second pole term denominator
};

struct parameters // Defining the parameters structure
{
    float ss_gain;           // Defining the steady state gain value
    float settling_time;     // Defining the settling time value
    float percent_overshoot; // Defining the percent overshoot value
};

struct controller // Defining the controller structure
{
    int type;           // Defining the controller type numerically
    float kp;           // Defining the proportional gain
    float kin;          // Defining the integral gain's numerator zero value
    float kid;          // Defining the integral gain's denominator value
    float kd;           // Defining the derivative gain's zero value
    float natural_freq; // Defining the natural frequency of the system used for the controller design
    float damp_ratio;   // Defining the damping ratio of the system used for the controller design
};

int main() // Main function
{
    int choice = 1;              // Defining the choice variable used to allow the user to design/view multiple controllers
    struct controller c1[20];    // Defining the controller structure array with 20 potential controllers
    struct transfer_function t1; // Defining the transfer function structure
    struct parameters p1;        // Defining the parameters structure array with 20 potential parameters' sets to correspond to each controller
    int i = 1;                   // Defining the i variable used to keep track of the current controller number       // Defining the input two variable used to clear the buffer
    do
    {
        int ans = 0; // Defining the ans variable used to allow the user to choose between designing a new controller or viewing a previous one
        if (i > 1)   // Allows the user to choose between view a previous controller or choose to make a new one if there is already at least one previous controller
        {
            do // Loops through at least once, as the do while is initiated
            {
                printf("Would you like to design a new controller or view a preious one? (new = 1/view = 2)\n"); // Asks the user if they would like to design a new controller or view a previous one, and gives the relevant numerical inputs
                if (scanf("%d", &ans) != 1)                                                                      // Prompts the user for an answer and checks the answer is numerical
                {
                    printf("Invalid input, please enter a number between 1 and 2\n"); // Error message thrown if the user enters an invalid input
                };
                clear(); // Clears the buffer
            } while (ans != 1 && ans != 2); // Loops through if the user enters any value that isnt 1 or 2
        }
        if (ans == 2) // If the user chooses to view a previous controller
        {
            int old; // Defining the variable to allow the user to specify which controller
            do       // Loops through at least once, as the do while is initiated
            {
                printf("Which controller do you want to view? (enter a value between 1 and %d)\n", i - 1); // Asks the user which controller they would like to view from the first to the current
                if (scanf("%d", &old) != 1)                                                                // Prompts the user for an answer and checks the answer is numerical
                {
                    printf("Invalid input, please enter a number between 1 and %d\n", i - 1); // Error message thrown if the user enters an invalid input
                }
                clear(); // Clears the buffer
            } while (old < 0 || old > i); // Loops through until the user enters a valid controller number from 0 to the curretn value of i
            controller_display(&c1[old], old); // calls the conroller display function to displaty the specified controller
        }
        else // If the user chooses to design a new controller
        {
            userInputs(&c1[i], i, &t1, &p1);
            printf("You have entered the folloing transfer function:\n");                                                                        // Calls the userInputs function to get the user inputs for the controller design
            printf("\t\t%f\n -----------------------------------------------\n (%.5f*s + %.5f)(%.5f*s + %.5f)\n", t1.a, t1.b, t1.c, t1.d, t1.e); // Displays the transfer function the user has just entered
            calcs(&c1[i], t1, &p1, i);                                                                                                           // Calls the calcs function to perform the calculations for the controller design based off of the previous user inputs
            controller_display(&c1[i], i);                                                                                                       // Calls the controller display function to display the controller the user has just requested
            i++;                                                                                                                                 // Increments the i variable to allow for the next controller to be designed
        };
        if (i < 20) // only asks the user if theuy are able to go again, if not the program is ended
        {
            do // Loops through at least once, as the do while is initiated
            {
                printf("\nWould you like to design another controller/view a previous one again? (yes = 1/no = 0)\n"); // Asks the user if they would like to go through again
                if (scanf("%d", &choice) != 1)                                                                         // Prompts the user for an answer and checks the answer is numerical
                {
                    printf("Invalid input, please enter either 1 and 2\n"); // Error message thrown if the user enters an invalid input
                };
                clear(); // Clears the buffer
            } while (choice != 1 && choice != 0); // Keeps the user looping till they enter a valid choice
        };
    } while (choice == 1); // Keeps iterating through if there is room for more controllers and the user choses to continue

    return 0; // Returns zero to signify the end of the program
}

float userInputs(struct controller *c1, int i, struct transfer_function *t1, struct parameters *p1) // Start of the userInputs function definition
{
    float step;                                                       // Initialising the value of the step input
    float alph;                                                       // Initialising the temporary alpha variable, used to calculate the numerator of the open loop system transfer function
    printf("Please enter the controller type you wish to design:\n"); // Asks the user to enter the controller type
    printf("PID = 1\nPI = 2\nPD = 3\n");                              // Details the numeric input correlating to each input
    do                                                                // Loops through at least once, as the do while is initiated
    {
        if (scanf("%d", &(c1[i]).type) != 1) // Prompts the user for an answer and checks the answer is numerical
        {
            printf("Invalid input, please enter either 1,2 or 3.\n"); // Error message thrown if the input is invalid
        };
        clear(); // Clears the buffer
    } while (c1[i].type != 1 && c1[i].type != 2 && c1[i].type != 3); // Keeps the user looping through until a valid answer is entered
    do // Loops through at least once, as the do while is initiated
    {
        printf("Please enter the value of the step input:\t"); // Asks the user to enter the value of their step input
        if (scanf("%f", &step) != 1)                           // Prompts the user for an answer and checks the answer is numerical
        {
            printf("Invalid input, please enter a number between -10000 and 10000\n"); // Error message thrown if the user enters an invalid input
        }
        clear(); // Clears the buffer
    } while (step < -10000 || step > 10000); // Keeps looping through
    printf("Please enter the transfer function of the system you wish to design the controller for:\n"); // Tells the user this is now the transfer function values they're entering
    printf("\ta\n ----------------\n (bs + c)(ds + e)\n");                                               // Shows the user the format of their entry
    printf("Note all values must be between -10000 and 10000\n");                                        // States the range of the values they must enter
    do                                                                                                   // Loops through at least once, as the do while is initiated
    {
        printf("a:\t");              // Asks the user to enter the value of the numerator
        if (scanf("%f", &alph) != 1) // Prompts the user for an answer and checks the answer is numerical
        {
            printf("Invalid input, please enter a number between -10000 and 10000\n"); // Error message thrown if the input is invalid

        }; // Prompts the user to enter the value of the numerator and stores in the temporary alth variable
        clear(); // Clears the buffer
    } while (alph > 10000 || alph < -10000); // Keeps looping through until a valid answer is entered
    t1->a = (alph * step); // Calculates the value of the numerator using the step input and alph vairable, and stores it in the transfer function structure's a value
    do                     // Loops through at least once, as the do while is initiated
    {
        printf("b:\t");               // Asks the user to enter the value of the first s term in the denominator
        if (scanf("%f", &t1->b) != 1) // Prompts the user for an answer and checks the answer is numerical
        {
            printf("Invalid input, please enter a number between -10000 and 10000\n"); // Error message thrown if the user enters an invalid input
        };
        clear(); // Clears the buffer
    } while (t1->b > 10000 || t1->b < -10000); // Keeps looping through until a valid answer is entered
    do // Loops through at least once, as the do while is initiated
    {
        printf("c:\t");               // Asks the user to enter the first pole
        if (scanf("%f", &t1->c) != 1) // Prompts the user for an answer and checks the answer is numerical
        {
            printf("Invalid input, please enter a number between -10000 and 10000\n"); // Error message thrown if the user enters an invalid input
        }; // Asks the user to enter the value of the first pole term in the denominator
        clear(); // Clears the buffer
    } while (t1->c > 10000 || t1->c < -10000); // Keeps looping through until a valid answer is entered
    do // Loops through at least once, as the do while is initiated
    {
        printf("d:\t");               // Asks the user to enter the value of the second s term in the denominator
        if (scanf("%f", &t1->d) != 1) // Prompts the user for an answer and checks the answer is numerical
        {
            printf("Invalid input, please enter a number between -10000 and 10000\n"); // Error message thrown if the user enters an invalid input
        }; // Prompts the user for their entry and stores it in the transfer function structure's d value
        clear(); // Clears the buffer
    } while (t1->d > 10000 || t1->d < -10000); // Keeps looping through until a valid answer is entered
    do // Loops through at least once, as the do while is initiated
    {
        printf("e:\t");               // Asks the user to enrer the value of the second pole
        if (scanf("%f", &t1->e) != 1) // Prompts the user for an answer and checks the answer is numerical
        {
            printf("Invalid input, please enter a number between -10000 and 10000\n"); // Error message thrown if the user enters an invalid input
        };
        clear(); // Clears the buffer
    } while (t1->e > 10000 || t1->e < -10000); // Keeps looping through until a valid answer is entered
    printf("Now you will be entering the desired charicteristics of the compensated system:"); // Tells the user they'll be entering the desired charicteristics of the system
    do                                                                                         // Loops through at least once, as the do while is initiated
    {
        printf("\nPlease enter the desired steady state gain of the system:\n"); // Asks the user to enter the desired steady state gain of the system
        if (scanf("%f", &p1->ss_gain) != 1)                                      // Prompts the user for an answer and checks the answer is numerical
        {
            printf("Invalid input, please enter a number between -10000 and 10000\n"); // Error message thrown if the user enters an invalid input
        };
        clear(); // Clears the buffer
    } while (p1->ss_gain > 10000 || p1->ss_gain < -10000); // Keeps looping through until a valid answer is entered
    do // Loops through at least once, as the do while is initiated
    {
        printf("Please enter the desired settling time of the system (in seconds):\n"); // Asks the user to enter the desired settlement time of the system in seconds
        if (scanf("%f", &p1->settling_time) != 1)                                       // Prompts the user for an answer and checks the answer is numerical
        {
            printf("Invalid input, please enter a number between -10000 and 10000\n"); // Error message thrown if the user enters an invalid input
        };
        clear(); // Clears the buffer
    } while (p1->settling_time > 10000 || p1->settling_time < -10000); // Keeps looping through until a valid answer is entered
    do // Loops through at least once, as the do while is initiated
    {
        printf("Please enter the desired percent overshoot of the system (As a decimal value between 0 and 1):\n"); // Asks the user to enter the desired percent overshoot of the system as a decimal value
        if (scanf("%f", &p1->percent_overshoot) != 1)                                                               // Prompts the user for an answer and checks the answer is numerical
        {
            printf("Invalid input, please enter a number between -10000 and 10000\n"); // Error message thrown if the user enters an invalid input
        };
        clear(); // Clears the buffer
    } while (p1->percent_overshoot < 0 || p1->percent_overshoot > 1); // Keeps looping through until a valid answer is entered
    return t1->a; // Returns the a component of the transfer function
    return t1->b; // Returns the b component of the transfer function
    return t1->c; // Returns the c component of the transfer function
    return t1->d; // Returns the d component of the transfer function
    return t1->e; // Returns the e component of the transfer function
}; // End of function definition for userInputs

void calcs(struct controller *c1, struct transfer_function t1, struct parameters *p1, int i) // Beginning of the calcs function definition
{
    float openP1 = t1.c * -1;                                                                                       // Calculates the open loop pole one value by multiplying the first pole by -1
    float openP2 = t1.e * -1;                                                                                       // Calculates the open loop pole two value by multiplying the second pole by -1
    printf("You have the following open loop poles: %f & %f", openP1, openP2);                                      // Calculates the open loop pole two value by multiplying the second pole by -1
    float damp = sqrt(1 / (1 + ((3.142 / log(p1[i].percent_overshoot)) * (3.142 / log(p1[i].percent_overshoot))))); // Finds the damping ratio of the system using the parameter's percent overshoot value
    float wn = 4 / (damp * p1[i].settling_time);                                                                    // Finds the natural frequency of the system using the parameter's settling time value
    (c1[i]).damp_ratio = damp;                                                                                      // Stores the damping ratio in the controller structure
    (c1[i]).natural_freq = wn;                                                                                      // Stores the natural frequency in the controller structure
    float real = -wn;                                                                                               // Calculates the real part of the desired poles
    float imag = ((wn)*sqrt(1 - damp * damp));                                                                      // Calculates the imaginary part of the desired poles
    printf("\nYour desired poles are: %f + %fi and %f - %fi\n", real, imag, real, imag);                            // Displays the desired poles to the user
    float arg1 = 180 - atan(imag / (real - t1.c));                                                                  // Calculates the first argument value using the desired pole one's real and imaginary parts, and the first pole from the transfer function
    float arg2 = 180 - atan(imag / (real - t1.e));                                                                  // Calculates the second argument value using the desired pole one's real and imaginary parts, and the second pole from the transfer function
    float deriv = ((imag / (-180 - arg1 - arg2)) + real);                                                           // Calculates the derivative's zero value using the desired pole one's real and imaginary parts, and the first and second arguments
    float l1 = sqrt(((real - t1.c) * (real - t1.c)) + ((imag) * (imag)));                                           // Calculates l1, which is the distance from pole one to desired pole one
    float l2 = sqrt(((real - t1.e) * (real - t1.e)) + ((imag) * (imag)));                                           // Calculates l2, which is the distance from pole two to desired ploe two
    float l3 = sqrt(((deriv - real) * (deriv - real)) + ((imag) * (imag)));                                         // Calculates l3, which is the distance from the derivative's zero to desired pole one
    float prop = ((l1 * l2) / (l3 * p1->ss_gain)) / t1.a;                                                           // Calculates the total K value, whihc is the value of the proportional gain
    (c1[i]).kp = prop;                                                                                              // Assigns this value to the controller structure's kp value

    if (c1[i].type == 1) // Run if the controller is a PID controller
    {
        (c1[i]).kid = 1;   // Gives the controller an integrator and assigns 1 to the controller structure's kid value
        (c1[i]).kin = 0.1; // Adds and additional zero to cancel out the integrator but be a small enough value as to not disrupt the root locus too much, this assigns 0.1 to the controller structure's kin value
        c1[i].kd = deriv;
    }
    else if (c1[i].type == 3) // Runs if the controller is a PD controller
    {
        c1[i].kd = deriv; // Assigns the derivative zero's value to the controller structure's kd value
    }
    else if (c1[i].type == 2) // Runs if the controller is a PI controller
    {
        (c1[i]).kid = 1;   // Gives the controller an integrator and assigns 1 to the controller structure's kid value
        (c1[i]).kin = 0.1; // Adds and additional zero to cancel out the integrator but be a small enough value as to not disrupt the root locus too much, this assigns 0.1 to the controller structure's kin value
    };
}; // End of function definition for calcs

void controller_display(struct controller *c1, int i) // Start fo the controller_display function definition
{
    printf("\nYou are viewing controller %d\n", i); // Tells the user which controller they're viewing
    if ((c1[i]).type == 1)                          // Runs if the controller is a PID controller
    {
        printf("Controller type: PID\n\n");                              // Tells the user what type of controller it is
        printf("%f (s + %f)(s + 0.1)\n", (c1[i]).kp, ((c1[i]).kd) * -1); // Sets the controllers transfer function's numerator
        printf("---------------------------------\n");                   // Adds a division line
        printf("                s\n");                                   // Adds the integrator
    }
    else if ((c1[i]).type == 2) // Runs if the controller is a PI controller
    {
        printf("Controller type: PI\n\n");       // Tells the user the type of controller they're viewing
        printf("%4.0f (s + 0.1)\n", (c1[i]).kp); // Sets the controllers transfer function's numerator
        printf("-----------------\n");           // Adds a division line
        printf("       s\n");                    // Adds the integrator
    }
    else if ((c1[i]).type == 3) // Runs if the controller is a PD controller
    {
        printf("Controller type: PD\n\n");                       // Tells the user the type of controller they're viewing
        printf("%4.0f (s + %f)\n", (c1[i]).kp, (c1[i]).kd * -1); // Displays the controllers transfer function
    };
}; // End of fucntion definition for controller_display

void clear(void) // Function definition for the clear function
{
    int c = 0; // Defining the input variable used to clear the buffer
    do         // Runs at least once
    {
        c = getchar(); // Gets the next character in the buffer
    } while (c != '\n'); // loops through until it reaches a new line
}; // End of function definition for clear