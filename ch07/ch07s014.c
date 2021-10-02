NODE *AddNode(NODE *root, char *data)
{
  diff = strcmp(data, root->data);
  if(diff > 0)
  {
    if(root->left = NULL)
    {
      root->left = malloc(sizeof *root->left);

/* ... */
