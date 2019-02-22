cups_array_t* generate_sizes(ipp_t *response,
                             ipp_attribute_t **defattr,
                             int* min_length,
                             int* min_width,
                             int* max_length,
                             int* max_width,
                             int* bottom,
                             int* left,
                             int* right,
                             int* top,
                             char* ppdname) 
{
  cups_array_t             *sizes;               /* Media sizes we've added */
  ipp_attribute_t          *attr,                /* xxx-supported */
                           *x_dim, *y_dim;       /* Media dimensions */
  ipp_t                    *media_col,           /* Media collection */
                           *media_size;          /* Media size collection */
  int                      i,count = 0;
  pwg_media_t              *pwg;                 /* PWG media size */

  if ((attr = ippFindAttribute(response, "media-bottom-margin-supported", IPP_TAG_INTEGER)) != NULL)
  {
    for (i = 1, *bottom = ippGetInteger(attr, 0), count = ippGetCount(attr); i < count; i ++)
      if (ippGetInteger(attr, i) > *bottom)
        *bottom = ippGetInteger(attr, i);
  }
  else
    *bottom = 1270;

  if ((attr = ippFindAttribute(response, "media-left-margin-supported", IPP_TAG_INTEGER)) != NULL)
  {
    for (i = 1, *left = ippGetInteger(attr, 0), count = ippGetCount(attr); i < count; i ++)
      if (ippGetInteger(attr, i) > *left)
        *left = ippGetInteger(attr, i);
  }
  else
    *left = 635;

  if ((attr = ippFindAttribute(response, "media-right-margin-supported", IPP_TAG_INTEGER)) != NULL)
  {
    for (i = 1, *right = ippGetInteger(attr, 0), count = ippGetCount(attr); i < count; i ++)
      if (ippGetInteger(attr, i) > *right)
        *right = ippGetInteger(attr, i);
  }
  else
    *right = 635;

  if ((attr = ippFindAttribute(response, "media-top-margin-supported", IPP_TAG_INTEGER)) != NULL)
  {
    for (i = 1, *top = ippGetInteger(attr, 0), count = ippGetCount(attr); i < count; i ++)
      if (ippGetInteger(attr, i) > *top)
        *top = ippGetInteger(attr, i);
  }
  else
    *top = 1270;

  if ((*defattr = ippFindAttribute(response, "media-col-default", IPP_TAG_BEGIN_COLLECTION)) != NULL)
  {
    if ((attr = ippFindAttribute(ippGetCollection(*defattr, 0), "media-size", IPP_TAG_BEGIN_COLLECTION)) != NULL)
    {
      media_size = ippGetCollection(attr, 0);
      x_dim      = ippFindAttribute(media_size, "x-dimension", IPP_TAG_INTEGER);
      y_dim      = ippFindAttribute(media_size, "y-dimension", IPP_TAG_INTEGER);

      if (x_dim && y_dim && (pwg = pwgMediaForSize(ippGetInteger(x_dim, 0), ippGetInteger(y_dim, 0))) != NULL)
  strlcpy(ppdname, pwg->ppd, PPD_MAX_NAME);
      else
  strlcpy(ppdname, "Unknown", PPD_MAX_NAME);
    }
    else
      strlcpy(ppdname, "Unknown", PPD_MAX_NAME);
  }
  else if ((pwg = pwgMediaForPWG(ippGetString(ippFindAttribute(response, "media-default", IPP_TAG_ZERO), 0, NULL))) != NULL)
    strlcpy(ppdname, pwg->ppd, PPD_MAX_NAME);
  else
    strlcpy(ppdname, "Unknown", PPD_MAX_NAME);

  sizes = cupsArrayNew3((cups_array_func_t)pwg_compare_sizes, NULL, NULL, 0, (cups_acopy_func_t)pwg_copy_size, (cups_afree_func_t)free);

  if ((attr = ippFindAttribute(response, "media-col-database", IPP_TAG_BEGIN_COLLECTION)) != NULL)
  {
    for (i = 0, count = ippGetCount(attr); i < count; i ++)
    {
      cups_size_t temp;   /* Current size */
      ipp_attribute_t *margin;  /* media-xxx-margin attribute */

      media_col   = ippGetCollection(attr, i);
      media_size  = ippGetCollection(ippFindAttribute(media_col, "media-size", IPP_TAG_BEGIN_COLLECTION), 0);
      x_dim       = ippFindAttribute(media_size, "x-dimension", IPP_TAG_ZERO);
      y_dim       = ippFindAttribute(media_size, "y-dimension", IPP_TAG_ZERO);
      pwg         = pwgMediaForSize(ippGetInteger(x_dim, 0), ippGetInteger(y_dim, 0));

      if (pwg)
      {
  temp.width  = pwg->width;
  temp.length = pwg->length;

  if ((margin = ippFindAttribute(media_col, "media-bottom-margin", IPP_TAG_INTEGER)) != NULL)
    temp.bottom = ippGetInteger(margin, 0);
  else
    temp.bottom = *bottom;

  if ((margin = ippFindAttribute(media_col, "media-left-margin", IPP_TAG_INTEGER)) != NULL)
    temp.left = ippGetInteger(margin, 0);
  else
    temp.left = *left;

  if ((margin = ippFindAttribute(media_col, "media-right-margin", IPP_TAG_INTEGER)) != NULL)
    temp.right = ippGetInteger(margin, 0);
  else
    temp.right = *right;

  if ((margin = ippFindAttribute(media_col, "media-top-margin", IPP_TAG_INTEGER)) != NULL)
    temp.top = ippGetInteger(margin, 0);
  else
    temp.top = *top;

  if (temp.bottom == 0 && temp.left == 0 && temp.right == 0 && temp.top == 0)
    snprintf(temp.media, sizeof(temp.media), "%s.Borderless", pwg->ppd);
  else
    strlcpy(temp.media, pwg->ppd, sizeof(temp.media));

  if (!cupsArrayFind(sizes, &temp))
    cupsArrayAdd(sizes, &temp);
      }
      else if (ippGetValueTag(x_dim) == IPP_TAG_RANGE || ippGetValueTag(y_dim) == IPP_TAG_RANGE)
      {
       /*
  * Custom size - record the min/max values...
  */

  int lower, upper;   /* Range values */

  if (ippGetValueTag(x_dim) == IPP_TAG_RANGE)
    lower = ippGetRange(x_dim, 0, &upper);
  else
    lower = upper = ippGetInteger(x_dim, 0);

  if (lower < *min_width)
    *min_width = lower;
  if (upper > *max_width)
    *max_width = upper;

  if (ippGetValueTag(y_dim) == IPP_TAG_RANGE)
    lower = ippGetRange(y_dim, 0, &upper);
  else
    lower = upper = ippGetInteger(y_dim, 0);

  if (lower < *min_length)
    *min_length = lower;
  if (upper > *max_length)
    *max_length = upper;
      }
    }
  }
  if ((attr = ippFindAttribute(response, "media-size-supported", IPP_TAG_BEGIN_COLLECTION)) != NULL)
  {
    for (i = 0, count = ippGetCount(attr); i < count; i ++)
    {
      cups_size_t temp;   /* Current size */

      media_size  = ippGetCollection(attr, i);
      x_dim       = ippFindAttribute(media_size, "x-dimension", IPP_TAG_ZERO);
      y_dim       = ippFindAttribute(media_size, "y-dimension", IPP_TAG_ZERO);
      pwg         = pwgMediaForSize(ippGetInteger(x_dim, 0), ippGetInteger(y_dim, 0));

      if (pwg)
      {
  temp.width  = pwg->width;
  temp.length = pwg->length;
  temp.bottom = *bottom;
  temp.left   = *left;
  temp.right  = *right;
  temp.top    = *top;

  if (temp.bottom == 0 && temp.left == 0 && temp.right == 0 && temp.top == 0)
    snprintf(temp.media, sizeof(temp.media), "%s.Borderless", pwg->ppd);
  else
    strlcpy(temp.media, pwg->ppd, sizeof(temp.media));

  if (!cupsArrayFind(sizes, &temp))
    cupsArrayAdd(sizes, &temp);
      }
      else if (ippGetValueTag(x_dim) == IPP_TAG_RANGE || ippGetValueTag(y_dim) == IPP_TAG_RANGE)
      {
       /*
  * Custom size - record the min/max values...
  */

  int lower, upper;   /* Range values */

  if (ippGetValueTag(x_dim) == IPP_TAG_RANGE)
    lower = ippGetRange(x_dim, 0, &upper);
  else
    lower = upper = ippGetInteger(x_dim, 0);

  if (lower < *min_width)
    *min_width = lower;
  if (upper > *max_width)
    *max_width = upper;

  if (ippGetValueTag(y_dim) == IPP_TAG_RANGE)
    lower = ippGetRange(y_dim, 0, &upper);
  else
    lower = upper = ippGetInteger(y_dim, 0);

  if (lower < *min_length)
    *min_length = lower;
  if (upper > *max_length)
    *max_length = upper;
      }
    }
  }
  if ((attr = ippFindAttribute(response, "media-supported", IPP_TAG_ZERO)) != NULL)
  {
    for (i = 0, count = ippGetCount(attr); i < count; i ++)
    {
      const char  *pwg_size = ippGetString(attr, i, NULL);
          /* PWG size name */
      cups_size_t temp, *temp2; /* Current size, found size */

      if ((pwg = pwgMediaForPWG(pwg_size)) != NULL)
      {
        if (strstr(pwg_size, "_max_") || strstr(pwg_size, "_max."))
        {
          if (pwg->width > *max_width)
            *max_width = pwg->width;
          if (pwg->length > *max_length)
            *max_length = pwg->length;
        }
        else if (strstr(pwg_size, "_min_") || strstr(pwg_size, "_min."))
        {
          if (pwg->width < *min_width)
            *min_width = pwg->width;
          if (pwg->length < *min_length)
            *min_length = pwg->length;
        }
        else
        {
    temp.width  = pwg->width;
    temp.length = pwg->length;
    temp.bottom = *bottom;
    temp.left   = *left;
    temp.right  = *right;
    temp.top    = *top;

    if (temp.bottom == 0 && temp.left == 0 && temp.right == 0 && temp.top == 0)
      snprintf(temp.media, sizeof(temp.media), "%s.Borderless", pwg->ppd);
    else
      strlcpy(temp.media, pwg->ppd, sizeof(temp.media));

    /* Add the printer's original IPP name to an already found size */
    if ((temp2 = cupsArrayFind(sizes, &temp)) != NULL) {
      snprintf(temp2->media + strlen(temp2->media),
         sizeof(temp2->media) - strlen(temp2->media),
         " %s", pwg_size);
      /* Check if we have also a borderless version of the size and add
         the original IPP name also there */
      snprintf(temp.media, sizeof(temp.media), "%s.Borderless", pwg->ppd);
      if ((temp2 = cupsArrayFind(sizes, &temp)) != NULL)
        snprintf(temp2->media + strlen(temp2->media),
         sizeof(temp2->media) - strlen(temp2->media),
         " %s", pwg_size);
    } else
      cupsArrayAdd(sizes, &temp);
  }
      }
    }
  }
  printf("before %s\n",ppdname);
  return sizes;
}
