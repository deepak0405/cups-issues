    if (max_width > 0 && min_width < INT_MAX && max_length > 0 && min_length < INT_MAX)
    {
      char	tmax[256], tmin[256];	/* Min/max values */

      _cupsStrFormatd(tleft, tleft + sizeof(tleft), left * 72.0 / 2540.0, loc);
      _cupsStrFormatd(tbottom, tbottom + sizeof(tbottom), bottom * 72.0 / 2540.0, loc);
      _cupsStrFormatd(tright, tright + sizeof(tright), right * 72.0 / 2540.0, loc);
      _cupsStrFormatd(ttop, ttop + sizeof(ttop), top * 72.0 / 2540.0, loc);

      cupsFilePrintf(fp, "*HWMargins: \"%s %s %s %s\"\n", tleft, tbottom, tright, ttop);

      _cupsStrFormatd(tmax, tmax + sizeof(tmax), max_width * 72.0 / 2540.0, loc);
      _cupsStrFormatd(tmin, tmin + sizeof(tmin), min_width * 72.0 / 2540.0, loc);
      cupsFilePrintf(fp, "*ParamCustomPageSize Width: 1 points %s %s\n", tmin, tmax);

      _cupsStrFormatd(tmax, tmax + sizeof(tmax), max_length * 72.0 / 2540.0, loc);
      _cupsStrFormatd(tmin, tmin + sizeof(tmin), min_length * 72.0 / 2540.0, loc);
      cupsFilePrintf(fp, "*ParamCustomPageSize Height: 2 points %s %s\n", tmin, tmax);

      cupsFilePuts(fp, "*ParamCustomPageSize WidthOffset: 3 points 0 0\n");
      cupsFilePuts(fp, "*ParamCustomPageSize HeightOffset: 4 points 0 0\n");
      cupsFilePuts(fp, "*ParamCustomPageSize Orientation: 5 int 0 3\n");
      cupsFilePuts(fp, "*CustomPageSize True: \"pop pop pop <</PageSize[5 -2 roll]/ImagingBBox null>>setpagedevice\"\n");
    }
  
