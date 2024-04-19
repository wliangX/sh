#pragma once


    vector<vector<int>> threeSum(vector<int>& nums) {
        if (nums.size() < 3) {
            return {};
        }

        for (vector<int>::iterator it = nums.begin(); it != nums.end(); it++) {
            cout << *it << " *** " << endl;
        }

        sort(nums.begin(), nums.end());
        if (nums[0] > 0 || nums[nums.size() - 1] < 0) {
            return {};
        }
        int size = nums.size();
        vector<vector<int>> ret;
        int temp = INT_MAX;
        for (int i = 0; i < size; i++) {
            cout << "LL " << nums[i] << endl;
            if (temp != nums[i]) { // nums[i] == nums[i - 1] // È¥ÖØ
                temp = nums[i];
            }
            else {
                continue;

            }
            int left = i + 1, right = size - 1;
            cout << "HH " << left << " " << right << endl;  // [-1,0,1,0] ¡¾-1 0 0 1¡¿
            while (left < right && left < size && right < size) {
                if (nums[i] + nums[left] + nums[right] == 0) {
                    cout << "ww " << nums[i] << " " << nums[left] << left << endl;
                    ret.push_back({ nums[i], nums[left], nums[right] });
                    left++;
                }
                else if (nums[i] + nums[left] + nums[right] > 0) {
                    right--;
                }
                else {
                    left++;
                }
                while (left < size - 1 && nums[left] == nums[left + 1]) {
                    left++;
                }
                while (nums[right] == nums[right - 1]) {
                    right--;
                }
            }
        }


        return ret;
    };


    bool repeatedSubstringPattern(string s) { // "abaababaab"
        int size = s.size();
        bool ret = false;
        int j;
        for (int len = 1; size % len == 0 && len <= size / 2; len++) {
            cout << len << " " << size << "  " << endl;
            for (j = len; j < size; j++) {
                cout << j << " " << j % len << " *** " << endl;
                if (s[j] != s[j % len]) {
                    ret = false;
                    break;
                }
                else {
                    ret = true;
                }
            }
            if (ret && j == size) {
                return true;
            }
        }
        return false;
    }





