#!/bin/bash

# Define a function to check the output against the expected result
check_output() {
    if diff <(sort -k1,1n <<< "$2") <(sort -k1,1n <<< "$3") >/dev/null; then
        echo "Case $1 => ● PASSED!"
        echo "Output:"
        echo "$2"
        echo "Expected:"
        echo "$3"
        ((passed_tests++))
    else
        echo "Case $1 => ● FAILED!"
        echo "Output:"
        echo "$2"
        echo "Expected:"
        echo "$3"
    fi
    echo "------------------------------------------------------------------------------------------------------------------------"
}

# Test cases
TEST_CASES=(
    "/root/"
    "/dev/null"
    "nonexist"
    "/proc/fs"
    "/usr/"
    "/proc/1/attr"
    "."
    "/boot"
    "/home/"
    "/media"
)

# Expected error messages
ERROR_MESSAGES=(
    "Permission denied. /root/ cannot be read."
    "The first argument has to be a directory."
    "Permission denied. nonexist cannot be read."
)

# Initialize the counter for passed tests
passed_tests=0

# Run test cases
for i in "${!TEST_CASES[@]}"; do
    OUTPUT=$(./sl "${TEST_CASES[i]}" 2>&1)
    if [ $i -lt 3 ]; then
        # Check for error messages
        if [[ "$OUTPUT" == "${ERROR_MESSAGES[i]}" ]]; then
            echo "Case $(($i + 1)) => ● PASSED!"
            ((passed_tests++))
        else
            echo "Case $(($i + 1)) => ● FAILED!"
        fi
        echo "Output:"
        echo "$OUTPUT"
        echo "Expected:"
        echo "${ERROR_MESSAGES[i]}"
    else
        # Check for sorted output
        EXPECTED=$(ls -ai "${TEST_CASES[i]}" | sort 2>&1)
        TOTAL_FILES=$(wc -l <<< "$EXPECTED") # Count the total number of files
        EXPECTED="$EXPECTED"$'\n'"Total files: $TOTAL_FILES" # Append the "Total files" line
        check_output "$(($i + 1))" "$OUTPUT" "$EXPECTED"
    fi
    echo "------------------------------------------------------------------------------------------------------------------------"
done

# Print the total number of passed tests
echo "Total passed tests: $passed_tests/${#TEST_CASES[@]}"
