// Name: Patrick Allison
// Date: Sept 24, 2010
// Program No.: 1.4.0
// Name: rpncalc.c
//
// Purpose: Creates an infix to RPN converter
// Solves equation from RPN formatted string
//
// Status Report
// Last Test Date: 09/24/2010 6:40 pm
// Addition of mulitple digit numbers including parenthesis is successful 
//
// Completed testing:
// a. floating point numbers
// b. signed numbers

// 
// Date: 09/24/2010 
// Added create rpn that includes -ve numbers
// Negative sign is represented as '_'     
//       e.g. 3 + _4 = evaluates to 3 _4 +
//       e.g. ( 3 + _4 ) = evaluates to 3 _4 +

// Input format: Ensure that there is a space after every operand and operator
// Example: ( 3 / 5 ) + ( 2 * 4 ) =



// Test Inputs									Test Outputs									Numeric value
// 3 + 4											3 4 +												7
// ( 3 + 4 )										3 4 +
// ( ( 3 + 4 ) * 5 )								3 4 + 5 *
// ( ( 3 * 4 ) + 5 )								3 4 * 5 +
// ( 3 + 4 * 5 )									3 4 5 * +
// 3 + 4 * 5										3 4 5 * +
// 3 * 4 + 5										3 4 * 5 +
// ( 3 * 4 + 5 )									3 4 * 5 +
// 3 + 5 * 6										3 5 6 * +
// 3 + ( 4 * 5 )									3 4 5 * +
// ( 3 / 5 ) + ( 2 * 4 )							3 5 / 2 4											8.6
// 3 * ( 4 + 5 )									3 4 5 + *
// ( 3 + 4 ) * 5									3 4 + 5 *
// ( 3 + 4 ) * ( 5 - 6 )							3 4 + 5 6 - *
// ( 3 + 4 ) * ( 5 - 6 ) / 7						3 4 + 5 6 - * 7 /
// ( ( 3 + ( 4 * 5 ) )								3 4 5 * +
// ( ( ( 3 + ( 4 * 5 ) ) )							3 4 5 * +
// ( 3 + 5 ) * ( 2 - 4 ) / 6						3 5 + 2 4 - * 6 /
// 3 / 4 + ( ( 5 - 4 ) / 6 * 7 )					3 4 / 5 4 - 6 / 7 * +
// ( 5 * 2 ) - ( ( 3 + 4 * 7 ) + 8 / 6 ) * 9		5 2 * 3 4 7 * + 8 6 / + 9 * -
// ( 5 * 2 ) - ( ( _3 + 4 * _7 ) + 8 / 6 ) * _9     5 2 * _3 4 _7 * + 8 6 / + _9 * -                    -257.000
// 12.3 + 4.5										12.3 4.5 +
// 3 + _4											3 _4 +
// ( 3 + _4 )										3 _4 +
// 12345 + 6789                                     12345 6789 +										19134.0
// 3 + 4 - _9                                       3 4 + _9 -                                          16
// 10.613 + 11.234									10.613 11.234 +										21.8470
// _10.623 + 11.234									_10.613 11.234 +									0.6210
// 6.3 / 3											6.3 3 /												2.1

//#import <Foundation/Foundation.h>

// operator priorities

#define MINUS			1
#define PLUS			1
#define DIV		        2
#define MUL			    2
#define SINE			3
#define RT_PAREN		0
#define LT_PAREN		0

#define TRUE			1
#define FALSE			0
#define EMPTY		   -1

void readInput(char word[], int* wordlength);
void showItems(char word[], int wordlength);
void createRPN(char word[], char rpn[], int wordlength, int* rpnPtr);
void computeRPN(char rpn[], int* rpnPtr);
void popStk(char stack[], char rpn[], int* stackPtr, int* rtop);                                 // use this prototype
void handleOperator(char stack[], char rpn[], int* stackPtr, int* rpntop, char item);            // use this prototype
int priority(char);

int main (int argc, const char * argv[])
{
	char word[20]   = {'\0'};                        // array of characters
	char rpn[20]    = {'\0'};                        // RPN expression
	int  wordlength = 0;
	int  rpnPointer = 0;                             // pointer to next open slot in RPN array
	char loopCntrl  = '\0';
	char newline    = '\0';
	
	do 
	{
		 readInput(word, &wordlength);
		
		 // DEBUG
		 showItems(word, wordlength);
		
		 createRPN(word, rpn, wordlength, &rpnPointer);
		 showItems(rpn, rpnPointer);
		 computeRPN(rpn, &rpnPointer);
		
		 printf("\nEnter another (y/n)?: ");
		 scanf("%c", &loopCntrl);
		 scanf("%c", &newline);
	 }   while (loopCntrl != 'n' ); 
	
	return 0;
	
} // end main

void readInput(char word[], int* wordlength)
{
	int i = 0;
	
	// get the word from the console one character at a time
	printf("\nEnter the word to display: ");
	
	word[i] = getchar();                             // get the first character from the keyboard
	
	while (word[i] != '\n')
	{
		i++;
		word[i] = getchar();		
	}
	word[i] = '\0';
	*wordlength = i;
	return;
}

void showItems(char word[], int wordlength)
{
	int i;
	
	// display the word to the console one character at a time
	printf("\nThis is the string: ");
	for(i = 0; i < wordlength; i++)
	{
		putchar(word[i]);
		//putchar(' ');
	}
	printf("\n");
	return;
}

void createRPN(char word[], char rpn[], int wordlength, int *rpnPtr)
{
	char stack[20] = {'\0'};
	char item;		
	BOOL isOperator;
	BOOL isDigit = NO;
	BOOL spaceON = FALSE;
	BOOL isSign = NO;                                  // -ve number sign
	int i = 0;
	int stkPointer = 0;                                // next empty slot on the stack 
	int rtop = EMPTY;
	
	// loop thru input string
	for (i = 0; i < wordlength; i++)
	{
		item = word[i];	
		isDigit = isdigit(item);
		
		switch (item)
		{
			case '(':
				isOperator = YES;
				break;
			case ')':
				isOperator = NO;                       // Not placed on stack so not considered operator                        
				popStk(stack, rpn, &stkPointer, &rtop);
				break;
			case '*':
				isOperator = YES;
				break;
			case '/':
				isOperator = YES;
			    break;
			case '+':
				isOperator = YES;
				break;
			case '-': 
				isOperator = YES;
				break;
			case '_':
				isSign = YES;
				isOperator = NO;
				break;
			case ' ':
				isOperator = NO;
				if (spaceON)                            // add a space at the end of the number                
				{
					rtop++;
					rpn[rtop] = item;
					spaceON = FALSE;
				}
				break;
			case '.':                                   // floating point value
				isOperator = NO;
				rtop++;
				rpn[rtop] = item;
				break;
			default:
				break;
		}
		if (isDigit)
		{
			rtop++;
			rpn[rtop] = item;
			spaceON = TRUE;                             // add a space after the number in the rpn array
		}
		else if (isOperator)
		{
			handleOperator(stack, rpn, &stkPointer, &rtop, item);
			isOperator = NO;
		}
		else if (isSign)
		{
			rtop++;
			rpn[rtop] = '_';
			isSign = NO;                                // reset
		}
			
	} // end for
	 
	// pop all remaining elements off stack, push to rpn queue
	popStk(stack, rpn, &stkPointer, &rtop);
	*rpnPtr = rtop + 1;		
	return;    	
} // end createRPN


void handleOperator(char stack[], char rpn[], int* stackPtr, int* rpntop, char item)
{
	int top = *stackPtr - 1;
	int rtop = *rpntop;
	
	if (top == EMPTY)
	{
		top++;
		stack[top] = item;                            // stack.push(item)  
	}
	else if (item == '(')
	{
		top++;
		stack[top] = item;                            // stack.push(item)
	}	
	else
	{
		if (priority(item) <= priority(stack[top]))        
		{
			rtop++;
			rpn[rtop] = stack[top];                   // rpn.push(top)
			rtop++;
			rpn[rtop] = ' ';                          // add a space after each operator
			stack[top] = item;                        // stack.push(item)
		}
		else
		{
			top++;
			stack[top] = item;                        // stack.push(item)
		}
	}
	*stackPtr = top + 1;                              // return pointer positions
	*rpntop   = rtop;
	return;	
} // end handleOperator


int priority(char item)
{
		int rank = 0;
		
		switch (item)
		{
			case '(':
				rank = LT_PAREN;
				break;
			case '*':
				rank = MUL;
				break;
			case '/':
				rank = DIV;
				break;
			case '+':
				rank = PLUS;
			    break;
			case '-':
				rank = MINUS;
				break;
			default:
				break;							
		}		
		return rank;
} // end priority


void popStk(char stack[], char rpn[], int* stackPtr, int* rpntop)
{	
	int i = 0;                                          // array index
	int top = *stackPtr - 1;
	int rtop = *rpntop;
	int numberStackItems = *stackPtr;                   // size of items on the stack
	
	// pop all elements off stack. 
	// Stop if left parenthesis found Push to rpn queue
	for (i = numberStackItems; i > 0; i--)
	{
		if (top == EMPTY)
		{
			return;
		}
		else if (stack[top] == '(')
		{
			top--;                                      // stack.pop(item)
			break;		                                // stop popping for left parenthesis	
		}		
		else
		{
			rtop++;                                     //rpn.push(top)
			rpn[rtop] = stack[top];
			rtop++;
			rpn[rtop] = ' ';                            // add a space after each operator
			top--;
		}						
	}	// end for
	*stackPtr = top + 1;
	*rpntop = rtop;	
	return;		
} // end popStk



void computeRPN(char rpn[], int *rpnPtr)
{
	float stack[20] = {0.0};
	int top = EMPTY;
	int size = *rpnPtr;
	int i = 0, digit = 0;
	float result = 0.0, decimal = 0.0;
	float factor = 1.0;
	char item = '\0';
	float operand1 = 0.0, operand2 = 0.0, realNumber = 0.0;
	BOOL isDigit = NO;
	BOOL digitFlag = NO;
	BOOL isSign = NO;
	BOOL isFloat = NO;
	int wholeNumber = 0;
	
	for (i = 0; i < size; i++)
	{
		item = rpn[i];	
		isDigit = isdigit(item);                     // identify numeric digits
		
		switch (item)
		{
			case '*':
				operand2 = stack[top];
				stack[top] = 0.0;                    // reset to an "empty" value
				top--;
				operand1 = stack[top];
				
				// compute intermediate result
				result = operand1 * operand2;
				stack[top] = result;
				break;
			case '/':
				operand2 = stack[top];
				stack[top] = 0.0;
				top--;
				operand1 = stack[top];
				if (operand2 == 0.0)
				{
					printf("\nERROR");
					printf("\noperand2 = 0");
					return;
				}
				else
				{   // compute intermediate result
					result = operand1 / (float)operand2;
					stack[top] = result;
				}	
			    break;
			case '+':
				operand2 = stack[top];
				stack[top] = 0.0;                     // reset to an "empty" value
				top--;
				operand1 = stack[top];
				
				// compute intermediate result
				result = operand1 + operand2;
				stack[top] = result;
				break;
			case '-':
				operand2 = stack[top];
				stack[top] = 0.0;                     // reset to an "empty" value
				top--;
				operand1 = stack[top];
				
				// compute intermediate result
				result = operand1 - operand2;
				stack[top] = result;
				break;
			case '_':
				isSign = YES;
				continue;
				break;
			case '.':
				isFloat = YES;
				continue;
				break;
			case ' ':
				if (digitFlag == YES)
				{
					if (isSign)
					{
						realNumber = -1 * realNumber;
						isSign = NO;
					}
					top++;
					stack[top] = realNumber;
					
					// reset 
					digitFlag = NO;
					isFloat = NO;
					realNumber = 0.0;
					decimal = 0.0;
					factor = 1.0;
					wholeNumber = 0;
				}
				break;
			default:
				break;							
		} // end switch
	
		if (isFloat)
		{
			factor = factor / 10.0;
			digit = (int)item - (int)'0';
			decimal = decimal + digit * factor;
			realNumber = wholeNumber + decimal;
		}
		else if (isDigit)
		{
			digitFlag = YES;
			digit = (int)item - (int)'0';
			wholeNumber = wholeNumber * 10 + digit;
			realNumber = wholeNumber;
		}				
	} // end for
	
	result = stack[top];
	printf("\nresult: %f", result);
	return;
}

