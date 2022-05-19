#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>
#include <memory>
#include <functional>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

/**
 * @brief qick solution of not having pow() --> <cmath> isn't uncluded
 *
 * @param number    --> integer
 * @param power   --> to what power do I want to raise the number
 * @return int      --> raised number
 */
int power(int number, int power)
{
    if (power == 0)
        return 1;

    int constant = number;
    for (int i = 1; i < power; i++)
        number = number * constant;

    return number;
}

/**
 * @brief coverts huffman encoded file to binary
 *
 * @param inFileName  --> path to encoded file
 * @param binary      --> referenced vector of @struct bits that takes in individual bites of encoded file a their position
 */
bool fileToBin(const char *inFileName, vector<bool> &binary)
{
    ifstream huf_file(inFileName, ios::in | ios::binary); // setting up stream of encoded file

    // error handling
    if (!huf_file.is_open())
        return false;

    // file empty check
    if (huf_file.peek() == std::ifstream::traits_type::eof())
        return false;

    size_t position = 0; // position in the file

    // read individual bites by bit shifting and masking
    char c;
    while (huf_file.get(c))
    {
        for (int i = 7; i >= 0; i--)
        {
            bool temp_binary = ((c >> i) & 1); // bit shift the char by int i - and use mask to determine if the bit is 0 || 1

            binary.push_back(temp_binary);
            position++;
        }
    }
    return true;
}

//==================================== CLASS TREE ================================

/**
 * @brief preorder binary tree used to extract codes for ASCII chars
 *
 */
class Tree
{
public:
    friend Tree *createTree(const vector<bool> &binary, int &binary_position);
    friend void deleteTree(Tree *root);
    friend uint printDecoded(const vector<bool> &binary, int &binary_position, Tree *root, string &ans);

private:
    Tree *m_Left = nullptr;
    Tree *m_Right = nullptr;
    char value = 0;
};

/**
 * @brief (recursively) create binary tree from given peorder encoded information
 *
 * @param binary    --> referenced vector of @struct bits that takes in individual bites of encoded file a their position
 * @return new_node --> recusively created node of @class Tree
 * @return   -->
 */
Tree *createTree(const vector<bool> &binary, int &binary_position)
{

    Tree *new_node = new Tree;

    if (binary[binary_position] == 0)
    {
        if (binary.size() > (uint)binary_position) // error handling --> case: file had badly encoded pre-ordered tree
            new_node->m_Left = createTree(binary, ++binary_position);

        if (binary.size() > (uint)binary_position) // error handling --> case: file had badly encoded pre-ordered tree
            new_node->m_Right = createTree(binary, ++binary_position);
    }

    else
    {
        // assigns character to a node value and shifts binary position respectively

        int char_int_value = 0;
        for (int i = 7; i >= 0; i--)
        {
            binary_position++;
            char_int_value = char_int_value + (binary[binary_position] * power(2, i));
            new_node->value = char_int_value;
        }
    }

    // error handling --> case: file had badly encoded pre-ordered tree
    if ((uint)binary_position >= binary.size())
    {
        deleteTree(new_node);
        return nullptr;
    }

    return new_node;
}

/**
 * @brief recursion delete function because of heap memory allocation previously done with 'new'
 *
 * @param root  --> root of huffman binary tree
 */
void deleteTree(Tree *root)
{
    if (root->m_Left != NULL)
        deleteTree(root->m_Left);

    if (root->m_Right != NULL)
        deleteTree(root->m_Right);

    delete root;
    return;
}

/**
 * @brief sub-funtion of @sa Tree::printDecoded  - sets @param chunk_value to how many chars there are left to read - one chunk is max 4096 chars (file can have more than one chunk)
 *
 * @param chunk_value       --> size of a current chunk max (4096)
 * @param binary            --> referenced vector of @struct bits that takes in individual bites of encoded file a their position
 * @param binary_position   --> position in the file after creating pre-order binary tree
 * @return 0                --> last chunk
 * @return 1                --> not the last chunk
 * @return 2                --> error
 */
uint chunkValue(uint &chunk_value, const vector<bool> &binary, int &binary_position)
{
    if (binary[binary_position] == 0)
    {
        // error handling --> case: expecting 12 characters to read the chunk but got less
        if ((binary.size() - binary_position) < 13)
            return 2;

        for (int i = 12; i >= 0; i--)
        {
            chunk_value = chunk_value + (binary[binary_position] * power(2, i));
            binary_position++;
        }
        return 1;
    }
    else
    {
        // error handling --> case: expecting atleast 1 character to read the chunk but got 0
        if (binary.size() - binary_position == 1)
            return 2;

        chunk_value = 4096;
        binary_position++;
    }
    return 0;
}

/**
 * @brief decodes the encoded part of the file
 *
 * @param binary            --> referenced vector of @struct bits that takes in individual bites of encoded file a their position
 * @param binary_position   --> position in the file after creating pre-order binary tree
 * @param root              --> root of huffman binary tree
 * @param ans               --> decoded string
 * @return uint             --> case: 0
 *                          --> case: 1
 *                          --> case: 2  error
 */
uint printDecoded(const vector<bool> &binary, int &binary_position, Tree *root, string &ans)
{
    Tree *curr = root;
    uint chunk_value = 0; // control how many chars can I read until bits from the file become invalid (are used as a filler of space)
    uint chunk_end = chunkValue(chunk_value, binary, binary_position);

    // error handling
    if (chunk_end == 2)
        return 2;

    for (size_t i = binary_position; i < binary.size(); i++)
    {
        if (chunk_value == 0)
            break;

        if (binary[i] == 0)
        {
            if (curr->m_Left == nullptr)
                return 2;
            curr = curr->m_Left;
        }
        else
        {
            if (curr->m_Left == nullptr)
                return 2;
            curr = curr->m_Right;
        }
        binary_position++;

        // reached leaf node
        if (curr->m_Left == nullptr && curr->m_Right == nullptr)
        {
            ans += curr->value;
            curr = root;
            chunk_value--;
        }
        if (binary.size() == (uint)binary_position && chunk_value != 0)
            return 2;
    }
    return chunk_end;
}

//==================================================================================
/**
 * @brief
 *
 * @param inFileName
 * @param outFileName
 * @return true
 * @return false
 */
bool decompressFile(const char *inFileName, const char *outFileName)
{
    ifstream in(inFileName, ios::in | ios::binary);
    if (!in.good())
    {
        return false;
    }

    vector<bool> binary;

    if (!fileToBin(inFileName, binary))
        return false;

    if (binary.size() % 8)
        return false; // erorr handling

    Tree *tree;
    int binary_position = 0;

    tree = createTree(binary, binary_position);
    if (tree == nullptr)
        return false;

    binary_position++; // calibration , last bit wasnt added in the function
    string decodedString;
    uint c = 0;
    while (c == 0)
    {
        c = printDecoded(binary, binary_position, tree, decodedString);
        if (c == 1)
            break;
        if (c == 2)
        {
            deleteTree(tree);
            return false;
        }
    }

    ofstream myfile(outFileName, ios::out);

    if (!myfile.is_open())
        return false;

    if (!myfile.good())
        return false;

    myfile << decodedString << flush;
    if (!myfile.flush())
        return false;
    myfile.close();

    deleteTree(tree);

    return true;
}

bool compressFile(const char *inFileName, const char *outFileName)
{
    // keep this dummy implementation (no bonus) or implement the compression (bonus)
    return false;
}
#ifndef __PROGTEST__

/**
 * @brief checks if sample decoded file and program output are the same
 *
 * @param fileName1     --> sample file
 * @param fileName2     --> program output
 * @return true         --> files are the same
 * @return false        --> files differ
 */
bool identicalFiles(const char *fileName1, const char *fileName2)
{

    unsigned char buffer1[1024], buffer2[1024];

    ifstream file1(fileName1, ios::in | ios::binary);
    if (!file1)
        return false;

    ifstream file2(fileName2, ios::in | ios::binary);
    if (!file2)
        return false;

    do
    {
        // buffering each file
        file1.read((char *)buffer1, sizeof(buffer1));
        file2.read((char *)buffer2, sizeof(buffer2));

        // checking number of bytes of each file
        if (file1.gcount() != file2.gcount())
        {
            file1.close();
            file2.close();
            return false;
        }

        // comparing buffer of each file one by one
        for (int i = 0; i < file1.gcount(); i++)
            if (buffer1[i] != buffer2[i])
            {
                file1.close();
                file2.close();
                return false;
            }

    } while (file1.good() && file2.good());

    file1.close();
    file2.close();

    return true;
}
//=============================================================================

int main(void)
{

    assert(!decompressFile("cisla.txt", "tempfile"));

    assert(decompressFile("tests/test0.huf", "tempfile"));
    assert(identicalFiles("tests/test0.orig", "tempfile"));

    assert(decompressFile("tests/test1.huf", "tempfile"));
    assert(identicalFiles("tests/test1.orig", "tempfile"));

    assert(decompressFile("tests/test2.huf", "tempfile"));
    assert(identicalFiles("tests/test2.orig", "tempfile"));

    assert(decompressFile("tests/test3.huf", "tempfile"));
    assert(identicalFiles("tests/test3.orig", "tempfile"));

    assert(decompressFile("tests/test4.huf", "tempfile"));
    assert(identicalFiles("tests/test4.orig", "tempfile"));

    assert(!decompressFile("tests/test5.huf", "tempfile"));

    assert(!decompressFile("tests/test7.huf", "tempfile"));

    // decompressFile("fail_tests/1", "tempfile");
    // decompressFile("fail_tests/2", "tempfile");
    // assert(decompressFile("fail_tests/1.txt", "tempfile"));
    // assert(decompressFile("dcode-file", "tempfile"));

    // assert ( decompressFile ( "tests/extra0.huf", "tempfile" ) );
    // assert ( identicalFiles ( "tests/extra0.orig", "tempfile" ) );

    // assert ( decompressFile ( "tests/extra1.huf", "tempfile" ) );
    // assert ( identicalFiles ( "tests/extra1.orig", "tempfile" ) );

    // assert ( decompressFile ( "tests/extra2.huf", "tempfile" ) );
    // assert ( identicalFiles ( "tests/extra2.orig", "tempfile" ) );

    // assert ( decompressFile ( "tests/extra3.huf", "tempfile" ) );
    // assert ( identicalFiles ( "tests/extra3.orig", "tempfile" ) );

    // assert ( decompressFile ( "tests/extra4.huf", "tempfile" ) );
    // assert ( identicalFiles ( "tests/extra4.orig", "tempfile" ) );

    // assert ( decompressFile ( "tests/extra5.huf", "tempfile" ) );
    // assert ( identicalFiles ( "tests/extra5.orig", "tempfile" ) );

    // assert ( decompressFile ( "tests/extra6.huf", "tempfile" ) );
    // assert ( identicalFiles ( "tests/extra6.orig", "tempfile" ) );

    // assert ( decompressFile ( "tests/extra7.huf", "tempfile" ) );
    // assert ( identicalFiles ( "tests/extra7.orig", "tempfile" ) );

    // assert ( decompressFile ( "tests/extra8.huf", "tempfile" ) );
    // assert ( identicalFiles ( "tests/extra8.orig", "tempfile" ) );

    // assert ( decompressFile ( "tests/extra9.huf", "tempfile" ) );
    // assert ( identicalFiles ( "tests/extra9.orig", "tempfile" ) );

    return 0;
}
#endif /* __PROGTEST__ */
