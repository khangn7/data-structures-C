#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// length of children[] array of Nodes
// for a-z and A-Z
#define N 52

#define MAX_WORD_SIZE 20

typedef struct
{
    char letter;

    // children[] will be in alphabetical order
    // 0 to 25 is a-z, 26 to 51 is A-Z
    struct Node* children[N]; 

    int is_word; // 1 when end, 0 when not
}
Node;

int addWord(Node* node, char* word);
void freeTrie(Node* root);
int findWord(Node* root, char* word);
unsigned int hash(char letter);

Node* getChildAtIndex(Node* parent, int index);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./trie file.txt\n");
        return 1;
    }

    printf("note: in your txt file, each bunch of chars followed by a '\\n' is interpreted\nas a word, so the last word also needs a '\\n' following it\n");

    FILE* input = fopen(argv[1], "r");
    if (input == NULL)
    {
        printf("invalid input file, fopen returned NULL");
        return 1;
    }

    Node* root = (Node*) malloc(sizeof(Node));
    if (root == NULL)
    {
        printf("malloc null 0");
        return 1;
    }
    // NULL init root->children
    for (int i = 0; i < N; i++)
    {
        root->children[i] = NULL;
    }

    // store words and when encounter '\n', add to trie
    char word[MAX_WORD_SIZE];
    int word_ind = 0;
    char c = 0;
    while (fread(&c, sizeof(char), 1, input))
    {
        if (c == '\n')
        {
            word[word_ind] = '\0';

            printf("%s\n", word);

            int success = addWord(root, (char*) word);
            printf("sucess %d\n", success);

            word_ind = 0;
        }
        else
        {
            word[word_ind] = c;
            word_ind++;
        }
    }

    printf("andy %d\n", findWord(root, "andy"));

    char* lebron = "lebron";

    printf("%s %d\n", lebron, findWord(root, lebron));

    printf("freeTrie()\n");

    freeTrie(root);

    return 0;
}

// returns 0 if Node added (mallocs succeeded) 1 if not
int addWord(Node* node, char* word)
{
    // if was passed nul terminator (word fully added last recursion)
    if (word[0] == '\0')
    {
        return 0;
    }

    unsigned int hashed = hash(word[0]);
    Node* child_node = getChildAtIndex(node, hashed);
    // printf("%p addWord()\n", child_node);

    // if current node doesn't have child representing word[0]
    if (child_node == NULL)
    {
        Node* new_node = (Node*) malloc(sizeof(Node));
        if (new_node == NULL)
        {
            return 1;
        }

        // set char
        new_node->letter = word[0];

        // NULL init children
        for (int i = 0; i < N; i++)
        {
            new_node->children[i] = NULL;
        }

        // if on last letter, set node->is_word to true
        if (word[1] == '\0')
        {
            new_node->is_word = 1;
        }
        // else set to 0
        else 
        {
            new_node->is_word = 0;
        }

        // add to parent->children
        node->children[hashed] = (struct Node*) new_node;

        // pointer arithmetic works here, strings like arrays?
        return addWord(new_node, word + 1);

    }
    // else if existing node->children whose letter is word[0]
    else
    {
        // if on last letter set child node->is_word == 1
        if (word[1] == '\0')
        {
            child_node->is_word = 1;
        }

        return addWord((Node*) node->children[hashed], word + 1);
    }

}

void freeTrie(Node* root)
{
    for (int i = 0; i < N; i++)
    {
        // will not call freeTrie on NULL ptrs
        // so no base case logic, free(NULL) will happen
        if (root->children[i] != NULL)
        {
            freeTrie((Node*) root->children[i]);
        }
    }
    free((Node*) root);
}

// return 0 if found word, 1 if not
int findWord(Node* root, char* word)
{
    unsigned int hashed = hash(word[0]);

    Node* child_node = (Node*) root->children[hashed];

    // note: function will never be passed '\0'

    // if first letter not found in current node
    if (child_node == NULL)
    {
        printf("child null at %s ", word);
        return 1;
    }
    
    // if on last letter
    if (word[1] == '\0')
    {
        // if child node->is_word == 1, indicates word exists
        if (child_node->is_word == 1)
        {
            // printf("last true ");
            return 0;
        }
        else
        {
            // printf("last false ");
            return 1;
        }
    }
    // else recurse, pass in word excluding first letter
    // caught by 1st if in function if next letter not valid
    else
    {
        return findWord(child_node, word + 1);
    }
}

Node* getChildAtIndex(Node* parent, int index)
{
    return (Node*) parent->children[index];
}

unsigned int hash(char letter)
{
    // islower(c) returns 0 if c is not lowercase
    int diff = isupper(letter) ? 26 : 0;
    return tolower(letter) - 'a' + diff;
}