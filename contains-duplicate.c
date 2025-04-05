#include <stdbool.h>
#include <stdio.h>

bool containsDuplicate(int *nums, int numsSize) {
  int temp = 0;
  for (int i = 0; i < numsSize; i++) {
    for (int j = i + 1; j < numsSize; j++) {
      if (nums[i] == nums[j]) {
        return true;
      }
    }
  }
  return false;
}

int main() {
  int nums[] = {1, 2, 4, 5, 8};
  if (containsDuplicate(nums, 6)) {
    printf("True");
  } else {
    printf("false");
  }
}
