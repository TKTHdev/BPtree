#include "bptree.h"
#include <vector>
#include <sys/time.h>

void
internal_distribute(TEMP *temp,NODE *left,NODE*right);

struct timeval
cur_time(void)
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return t;
}

void
print_tree_core(NODE *n)
{
    printf("[");
    for (int i = 0; i < n->nkey; i++) {
	if (!n->isLeaf) print_tree_core(n->chi[i]); 
	printf("%d", n->key[i]); 
	if (i != n->nkey-1 && n->isLeaf) putchar(' ');
    }
    if (!n->isLeaf) print_tree_core(n->chi[n->nkey]);
    printf("]");
}

void
print_tree(NODE *node)
{
    print_tree_core(node);
    printf("\n"); fflush(stdout);
}

NODE *
find_leaf(NODE *node, int key)
{
    int kid;

    if (node->isLeaf) return node;
    for (kid = 0; kid < node->nkey; kid++) {
	if (key < node->key[kid]) break;
    }

    return find_leaf(node->chi[kid], key);
}

NODE *
insert_in_leaf(NODE *leaf, int key, DATA *data)
{
    int i;
    if (key < leaf->key[0]) {
	for (i = leaf->nkey; i > 0; i--) {
	    leaf->chi[i] = leaf->chi[i-1] ;
	    leaf->key[i] = leaf->key[i-1] ;
	} 
	leaf->key[0] = key;
	leaf->chi[0] = (NODE *)data;
    }
    else {
	for (i = 0; i < leaf->nkey; i++) {
	    if (key < leaf->key[i]) break;
	}


	for (int j = leaf->nkey;j>i; j--) {
	    leaf->chi[j] = leaf->chi[j-1] ;
	    leaf->key[j] = leaf->key[j-1] ;
	} 
	leaf->key[i] = key;
	leaf->chi[i] = (NODE *)data;
    
    }
    leaf->nkey++;


    return leaf;
}

NODE *
alloc_leaf(NODE *parent)
{
    NODE *node;
    if (!(node = (NODE *)calloc(1, sizeof(NODE)))) ERR;
    node->isLeaf = true;
    node->parent = parent;
    node->nkey = 0;

    return node;
}

TEMP*
internal_copy_to_temp(TEMP *temp, NODE *node,NODE* child,int key)
{
    temp->nkey = N - 1;
    for (int i = 0; i < node->nkey; i++)
    {
        temp->key[i] = node->key[i];
        temp->chi[i] = node->chi[i];
    }
    temp->chi[N - 1] = node->chi[N - 1];
    for (int i = 0; i < N; i++)
    {
        node->chi[i] = NULL;
        node->key[i] = 0;
    }
    int i;
    if (key < temp->key[0]) {
        for (i = temp->nkey; i > 0; i--) {
            temp->chi[i+1] = temp->chi[i] ;
            temp->key[i] = temp->key[i-1] ;
        }
        temp->key[0] = key;
        temp->chi[1] = (NODE *)child;
    }
    else {

        for (i = 0; i < temp->nkey; i++) {
            if (key < temp->key[i]) break;
        }


        for (int j = temp->nkey;j>i; j--) {
            temp->chi[j+1] = temp->chi[j] ;
            temp->key[j] = temp->key[j-1] ;
        }
        temp->key[i] = key;
        temp->chi[i+1] = (NODE *)child;
    }
    temp->nkey++;
    return temp;
}




TEMP *
insert_temp(TEMP *temp, NODE *leaf,DATA *data,int key)
{
    temp->nkey = N-1;
    //copy data to temp
    for(int i=0;i<temp->nkey;i++)
    {
        temp->chi[i] = leaf->chi[i];
        temp->key[i] = leaf->key[i];
    }
    temp->chi[N-1]=leaf->chi[N-1];

    //reset the leaf
    for(int i=0;i<N;i++)
    {
        leaf->chi[i] = NULL;
        leaf->key[i] = 0;
    }


    int i;
    if (key < temp->key[0]) {
        for (i = temp->nkey; i > 0; i--) {
            temp->chi[i] = temp->chi[i-1] ;
            temp->key[i] = temp->key[i-1] ;
        }
        temp->key[0] = key;
        temp->chi[0] = (NODE *)data;
    }
    else {

        for (i = 0; i < temp->nkey; i++) {
            if (key < temp->key[i]) break;
        }


        for (int j = temp->nkey;j>i; j--) {
            temp->chi[j] = temp->chi[j-1] ;
            temp->key[j] = temp->key[j-1] ;
        }
        temp->key[i] = key;
        temp->chi[i] = (NODE *)data;
    }
    temp->nkey++;
    return temp;
}

void
internal_resolve(TEMP *temp,NODE *leaf,NODE *right)
{
    if (leaf == Root)
    {
        NODE *newRoot = alloc_leaf(NULL);
        Root = newRoot;
        Root->isLeaf = false;
        Root->nkey = 1;
        Root->key[0] = temp->key[temp->nkey/2];
        Root->chi[0] = leaf;
        Root->chi[1]=right;
        leaf->parent = Root;
        right->parent = Root;
    }
    else
    {
        if(leaf->parent->nkey<N-1)
        {
            if (right->key[0] < leaf->parent->key[0])
            {
                for (int i = leaf->parent->nkey; i > 0; i--)
                {
                    leaf->parent->chi[i + 1] = leaf->parent->chi[i];
                    leaf->parent->key[i] = leaf->parent->key[i - 1];
                }

                leaf->parent->key[0] = temp->key[temp->nkey/2];
                leaf->parent->chi[0] = leaf;
                leaf->parent->chi[1] = right;
                leaf->parent->nkey++;
            }
            else
            {
                int i;
                for (i = 0; i < leaf->parent->nkey; i++)
                {
                    if (right->key[0] < leaf->parent->key[i])break;
                }
                for (int j = leaf->parent->nkey; j > i; j--)
                {
                    leaf->parent->chi[j + 1] = leaf->parent->chi[j];
                    leaf->parent->key[j] = leaf->parent->key[j - 1];
                }
                leaf->parent->key[i] = temp->key[temp->nkey/2];
                leaf->parent->chi[i] = leaf;
                leaf->parent->chi[i + 1] = right;
                leaf->parent->nkey++;
            }
            right->parent = leaf->parent;
        }
        else
        {
            TEMP tem;
            NODE* nxt = alloc_leaf(NULL);
            nxt->isLeaf=false;
            nxt->parent = leaf->parent->parent;
            internal_copy_to_temp(&tem,leaf->parent,right,right->key[0]);
            internal_distribute(&tem,leaf->parent,nxt);
            internal_resolve(&tem,leaf->parent,nxt);
        }
    }

    for(int i=0;i<=right->nkey;i++)right->chi[i]->parent = right;
}

void
internal_distribute(TEMP *temp,NODE *left,NODE*right)
{
    for (int i = 0; i < temp->nkey / 2; i++)
    {
        left->key[i] = temp->key[i];
        left->chi[i] = temp->chi[i];
    }
    left->chi[temp->nkey / 2] = temp->chi[temp->nkey / 2];
    left->chi[temp->nkey/2]->parent = left;

    for (int i = temp->nkey / 2 + 1; i < temp->nkey; i++)
    {
        right->key[i-(temp->nkey / 2 + 1)] = temp->key[i];
        right->chi[i-(temp->nkey / 2 + 1)] = temp->chi[i];
    }
    right->chi[temp->nkey-((temp->nkey / 2 + 1))] = temp->chi[temp->nkey];
    left->nkey = temp->nkey/2;
    right->nkey = temp->nkey/2-1;
}

void
distribute(TEMP *temp, NODE *leaf, NODE *newLeaf)
{
    int mid = N/2;
    for(int i=0;i<mid;i++)
    {
        leaf->chi[i] = temp->chi[i];
        leaf->key[i] = temp->key[i];
    }
    for(int i=mid;i<N;i++)
    {
        newLeaf->chi[i-mid] = temp->chi[i];
        newLeaf->key[i-mid] = temp->key[i];
    }
    leaf->nkey = mid;
    newLeaf->nkey = N-mid;
    return ;
}

void 
resolve(NODE* leaf,NODE* newLeaf)
{
    if (leaf->parent == NULL)
    {
        NODE * newRoot = alloc_leaf(NULL);
        Root = newRoot;
        Root->isLeaf = false;
        Root->nkey = 1;
        Root->key[0] = newLeaf->key[0];
        Root->chi[0] = leaf;
        Root->chi[1] = newLeaf;
        newLeaf->parent = Root;
        leaf->parent = Root;
        return ;
    }
    else
    {
        if(newLeaf->parent->nkey <N-1)
        {
            if (newLeaf->key[0] < leaf->parent->key[0])
            {
                for (int i = leaf->parent->nkey; i > 0; i--)
                {
                    leaf->parent->chi[i + 1] = leaf->parent->chi[i];
                    leaf->parent->key[i] = leaf->parent->key[i - 1];
                }

                leaf->parent->key[0] = newLeaf->key[0];
                leaf->parent->chi[0] = leaf;
                leaf->parent->chi[1] = newLeaf;
                leaf->parent->nkey++;
            }
            else
            {
                int i;
                for (i = 0; i < leaf->parent->nkey; i++)
                {
                    if (newLeaf->key[0] < leaf->parent->key[i])break;
                }

                for (int j = leaf->parent->nkey; j > i; j--)
                {
                    leaf->parent->chi[j + 1] = leaf->parent->chi[j];
                    leaf->parent->key[j] = leaf->parent->key[j - 1];
                }
                leaf->parent->key[i] = newLeaf->key[0];
                leaf->parent->chi[i] = leaf;
                leaf->parent->chi[i + 1] = newLeaf;
                leaf->parent->nkey++;

            }
        }

        else
        {
            TEMP tem;
            NODE* right = alloc_leaf(NULL);
            right->isLeaf=false;
            right->parent=leaf->parent;
            internal_copy_to_temp(&tem,leaf->parent,newLeaf,newLeaf->key[0]);
            internal_distribute(&tem,leaf->parent,right);
            internal_resolve(&tem,leaf->parent,right);
        }
    }
}

void insert(int key, DATA *data)
{
    NODE *leaf;

    if (Root == NULL)
    {
        leaf = alloc_leaf(NULL);
        Root = leaf;
    } else
    {
        leaf = find_leaf(Root, key);
    }
    if (leaf->nkey < (N - 1))
    {
        insert_in_leaf(leaf, key, data);
    }
    else
    {
        // split
        // future work
        TEMP temp;
        insert_temp(&temp, leaf, data, key);
        NODE *newLeaf = alloc_leaf(NULL);
        leaf->chi[N - 1] = newLeaf;
        newLeaf->parent = leaf->parent;
        distribute(&temp, leaf, newLeaf);
        resolve(leaf,newLeaf);
    }
}

void
init_root(void)
{
    Root = NULL;
}

int
interactive()
{
    int key;

    cout << "Key: ";
    cin >> key;

    return key;
}

int
main(int argc, char *argv[])
{
    struct timeval begin, end;

    init_root();

    printf("-----Insert-----\n");
    begin = cur_time();
    for(int i=1000000;i>=1;i--)
    {
        insert(rand()%10000000, NULL);
        //print_tree(Root);
    }
    print_tree(Root);
    end = cur_time();

    return 0;
}