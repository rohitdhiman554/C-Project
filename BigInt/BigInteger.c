#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include "D:\BigInt\utilities.h"
#include "D:\BigInt\BigInteger.h"
#define MAX_INT_LENGTH 100

struct BigInt {
  int sign, numDigits;
  char number[MAX_INT_LENGTH+1];
};

// Returns a new BigInt of value 0
BigInteger* getNewBigInt() {
  BigInteger *bi = malloc(sizeof(BigInteger));
  //Fill with zeros
  for (int i = 0; i < MAX_INT_LENGTH; ++i)
    bi->number[i] = '0';
  bi->numDigits = 0;
  bi->sign = 1;
  bi->number[MAX_INT_LENGTH] = '\0';
  return bi;
}

// Parses the number passed as a string and returns a new BigInt.
// Returns NULL if unable to parse
BigInteger* parseToBigInt(char *string) {
  int length = strlen(string), signEncountered = 0, i;
  if (length > MAX_INT_LENGTH)
    return NULL;
  BigInteger *destination = getNewBigInt();
  for (int i = length-1; i >= 0; --i) {
    if (isdigit(string[i])) {
      (destination->numDigits)++;
      destination->number[MAX_INT_LENGTH - (destination->numDigits)] = string[i];
    }
    else if (sign(string[i])) {
      if (signEncountered) {
        // already sign was encountered once
        free(destination);
        return NULL;
      }
      signEncountered = 1;
      destination->sign = sign(string[i]);
    }
    else {
      // If the char is neither digit or sign, return NULL
      free(destination);
      return NULL;
    }
  }


  // Delete leading zeros if inserted
  while (destination->numDigits > 0 && destination->number[MAX_INT_LENGTH-destination->numDigits] == '0') {
        (destination->numDigits)--;

  }

  return destination;
}

// Returns  1 if |a| has the highest value
// Returns -1 if |b| has the highest value
// Returns  0 if |a| == |b|
int compareBigIntMagnitude(BigInteger *a, BigInteger *b) {
  if (a->numDigits > b->numDigits)
    return 1;
  else if (a->numDigits < b->numDigits)
    return -1;
  else {


    return 0;
  }
}


// Returns  1 if 'a' is the largest
// Returns -1 if 'b' is the largest
// Returns  0 if 'a' == 'b'
int compareBigInt(BigInteger *a, BigInteger *b) {
  if (a->sign > b->sign) return 1;
  else if (a->sign < b->sign) return -1;
  else {
    int mag = compareBigIntMagnitude(a, b);
    if(!mag) return 0;
    if((a->sign == 1 && mag == 1) || (a->sign == -1 && mag == -1))
      return 1;
    else
      return -1;
  }
}


// A function that can be used as a hook to get the internal values
// of the given BigInt. The function Pointer 'fnPtr' points to a
// function implemented by the caller program that in turn gets
// all the internal values of BigInt.
void BigInt_map(BigInteger *res, void (*fnPtr)(char *str, int n, int sign))
{
  assert (res);
  (*fnPtr)(res->number, res->numDigits, res->sign);

}

// Add the two given numbers and returns the pointer
// to the sum (which has to be free()'ed explicitly)
BigInteger* addBigInt(BigInteger *a, BigInteger *b) {
  assert(a && b);
  int sign = (a->sign == b->sign) ? a->sign : 0;
  // If a & b have opposite signs, we can better perform subtraction
  if (!sign) {
    BigInteger *neg = (a->sign == -1) ? a : b;
    BigInteger *pos = (a->sign == 1) ? a : b;
    neg->sign = 1;
    BigInteger *tmp = subtractBigInt(pos, neg);
    neg->sign = -1;
    return tmp;
  }
  BigInteger *result = getNewBigInt();
  result->sign = sign;
  int carry = 0, i, tmp, processTill;
  tmp = compareBigIntMagnitude(a, b);
  // The inputs have to be processed till the longest value is processed
  processTill = (tmp == 1) ? a->numDigits : b->numDigits;//equal case ?
  processTill = MAX_INT_LENGTH - processTill;
  for (i = MAX_INT_LENGTH-1; i >= processTill; --i) {
    tmp = a->number[i] + b->number[i] - 2*'0' + carry;
    carry = (tmp > 9) ? 1 : 0;
    result->number[i] = tmp%10 + '0';
    result->numDigits++;
  }
  if (carry) {
    result->number[i--] = '1';
    carry = 0;
  }
  return result;
}

// Subtract the two given numbers and returns the pointer
// to the difference (which has to be free()'ed explicitly)
BigInteger* subtractBigInt(BigInteger *a, BigInteger *b) {
  assert(a && b);
  int sign = (a->sign != b->sign) ? ((a->sign == 1) ? 1 : -1) : 0;
  // If a & b have different signs, we can better perform addition
  if (sign) {
    BigInteger *neg = (a->sign == 1) ? b : a;
    BigInteger *pos = (a->sign == 1) ? a : b;
    neg->sign = 1;
    BigInteger *tmp = addBigInt(pos, neg);
    tmp->sign = sign;
    neg->sign = -1;
    return tmp;
  }
  BigInteger *result = getNewBigInt();
  int borrow = 0, i;
  int tmp = compareBigIntMagnitude(a, result);
  // If a==0, return -b
  if (!tmp) {
    memcpy(result, b, sizeof(BigInteger));
    result->sign *= -1;
    return result;
  }
  tmp = compareBigIntMagnitude(b, result);
  // If b==0, return a
  if (!tmp) {
    memcpy(result, a, sizeof(BigInteger));
    return result;
  }
  tmp = compareBigIntMagnitude(a, b);
  // If a==b, return 0
  if (!tmp) return result;
  BigInteger *big = (tmp == 1) ? a : b;
  BigInteger *small = (tmp == 1) ? b : a;
  result->sign = (tmp == big->sign) ? 1 : -1;
  // The inputs have to be processed till the longest value is processed
  int processTill = MAX_INT_LENGTH - big->numDigits;
  for (i = MAX_INT_LENGTH-1; i >= processTill; --i) {
    tmp = big->number[i] - small->number[i] - borrow;
    if (tmp < 0) {
      result->number[i] = tmp + 10 + '0';
      borrow = 1;
    } else {
      result->number[i] = tmp + '0';
      borrow = 0;
    }
    result->numDigits++;
  }
  // Ignore leading zeros
  while (result->numDigits > 0 && result->number[MAX_INT_LENGTH-result->numDigits] == '0') {
        (result->numDigits)--;
      }
  assert (borrow == 0 /* Something wrong with the implementation */);
  return result;
}

// A helper function that multiplies the given one digit number with the BigInt, and
// shifts the result by 'shiftBy' value and store it in destination.
// Returns 0 only if the operation was successful.
int oneDigitMultiply(int n, BigInteger *num, int shiftBy, BigInteger *destination) {
  assert (n>=0 && n<=9 && shiftBy>=0 && destination);
  int carry = 0, i, j, tmp, processTill = MAX_INT_LENGTH - shiftBy;
  printf("shiftBy %d\n",shiftBy);
  printf("processTill %d\n",processTill);
  // Shift the destination by the given value (by filling that many zeros)
  for (j = MAX_INT_LENGTH-1; j >= processTill; --j)
  {
    destination->number[j] = '0';
  }
  destination->numDigits = shiftBy;
  processTill = MAX_INT_LENGTH - num->numDigits;
  printf("j=%d",j);
  printf("NEW processTill %d\n",processTill);
  for (i = MAX_INT_LENGTH-1; i >= processTill && j >= 0; --i, --j) {
    tmp = n * (num->number[i] - '0') + carry;
    carry = tmp/10;
    destination->number[j] = tmp%10 + '0';
    destination->numDigits++;
  }
  if (carry && j >= 0) {
    destination->number[j--] = carry + '0';
    destination->numDigits++;
  }
  // If insufficient place to store result, product means it has overflowed
  if (j < 0 && i >= processTill) return -1;
  // Fill remaining with zero
  for (; j >= 0; j--) destination->number[j] = '0';
  printf("destination of number %d \n",destination->number[7]);
  return 0;
}

// Multiply the two given numbers and returns the pointer
// to the product (which has to be free()'ed explicitly)
BigInteger* multiplyBigInt(BigInteger *a, BigInteger *b) {
  assert(a && b);
  BigInteger *result = getNewBigInt();
  int tmp;
  // If a==0, return 0
  tmp = compareBigIntMagnitude(a, result);
  if (!tmp) return result;
  // If b==0, return 0
  tmp = compareBigIntMagnitude(b, result);
  if (!tmp) return result;
  // Find which number is big in magnitude so that each digit in small
  // number is multiplied with the big one
  tmp = compareBigIntMagnitude(a, b);
  BigInteger *big = (tmp == 1) ? a : b;
  BigInteger *small = (tmp == 1) ? b : a;
  int limit = MAX_INT_LENGTH - small->numDigits;
  BigInteger *tmpSum = getNewBigInt(), *old;
  for (int i = MAX_INT_LENGTH - 1; i >= limit; --i) {
    tmp = oneDigitMultiply(small->number[i]-'0', big, MAX_INT_LENGTH-1 - i, tmpSum);
    if (tmp) {
      // Return NULL if the operation failed
      free(tmpSum);
      free(result);
      return NULL;
    }
    old = result;
    result = addBigInt(result, tmpSum);
    free(old);
  }
  result->sign = a->sign * b->sign;
  return result;
}

