class Solution(object):
    def twoSum(self, nums, target):
        """
        :type nums: List[int]
        :type target: int
        :rtype: List[int]
        """
        iterated = {}
        for i , num in enumerate(nums):
            complement= target-num
            if complement in iterated:
                return [iterated[complement], i]
            iterated[num] = i





            

            
        
