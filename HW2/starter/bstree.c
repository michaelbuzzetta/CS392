/*******************************************************************************
 * Name        : bstree.c
 * Author      : Michael Buzzetta
 * Pledge      : I pledge my honor that I have abided by the Stevens honor system
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bstree.h"
#include "utils.h"

/*
Recieved help from CAs during office hours:
Eddison So
Daniel Zamloot
Rudolph Sedlin
*/

void add_node(void *data, size_t size, tree_t *tree, int (*compare)(void *, void *))
{
    if (tree->root == NULL)
    {

        tree->root = (node_t *)malloc(sizeof(node_t));
        if (tree->root == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            return;
        }

        tree->root->data = malloc(size);
        if (tree->root->data == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            return;
        }

        for (size_t i = 0; i <= size; i++)
        {
            *((char *)(tree->root->data) + i) = *((char *)(data) + i);
        }
        // printf("%d", *((int*)tree->root->data));
        // memcpy(tree->root->data, data, size);
        tree->root->left = NULL;
        tree->root->right = NULL;
    }
    else
    {
        node_t *temp = tree->root;
        while (1)
        {
            if (temp == NULL)
            {
                return;
            }

            int compare1 = (*compare)(data, temp->data);

            if (compare1 < 0)
            {
                if (temp->left == NULL)
                {
                    printf("3 ");
                    temp->left = (node_t *)malloc(sizeof(node_t));

                    temp->left->data = malloc(size);

                    for (size_t i = 0; i <= size; i++)
                    {
                        *((char *)(temp->left->data) + i) = *((char *)(data) + i);
                    }
                    temp->left->left = NULL;
                    temp->left->right = NULL;
                    return;
                }
                temp = temp->left;
            }
            else
            {
                if (temp->right == NULL)
                {

                    temp->right = (node_t *)malloc(sizeof(node_t));

                    temp->right->data = malloc(size);

                    for (size_t i = 0; i <= size; i++)
                    {
                        *((char *)(temp->right->data) + i) = *((char *)(data) + i);
                    }
                    temp->right->left = NULL;
                    temp->right->right = NULL;
                    return;
                }
                temp = temp->right;
            }
        }
    }
}

void print_tree(node_t *var, void (*print_detect)(void *))
{
    if (var == NULL)
    {
        return;
    }

    print_tree(var->left, print_detect);
    print_detect(var->data);
    print_tree(var->right, print_detect);
}

void destroy_helper(node_t *curr)
{
    if (curr == NULL)
    {
        return;
    }

    destroy_helper(curr->left);
    destroy_helper(curr->right);
    free(curr->data);
    free(curr);
    // curr->data = NULL;
    // curr = NULL;
}

void destroy(tree_t *tree)
{
    destroy_helper(tree->root);
    // free(tree->root);
    tree->root = NULL;
}
