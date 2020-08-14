# include <stdio.h>
# include <stdlib.h>
# include <string.h>

int main(unsigned int argc, char * argv[])
{
  # define ERR_FILE_NOT_FOUND 15
  # define UNKNOWN_FILE_TYPE 63

  /*  define DEL_TMP_OUT_FILES if you want to delete temporary files.  */
  /*  # define DEL_TMP_OUT_FILES  */

  typedef enum {False, True} Bool;
  const double HartreeTokJpermol = 2625.499189428056;
  char cont = '\0';
  FILE * fl = NULL;
  char line[BUFSIZ] = "";
  char comm[BUFSIZ] = "";
  char fname[BUFSIZ] = "";
  char fchname[BUFSIZ] = "";
  char newname[BUFSIZ] = "";
  unsigned int lpos = 0;
  unsigned int fnleng = 0;
  char * appname = NULL;
  double temp = 298.15;
  double pres = 1.0;
  double sca_ZPE = 0.9813;
  double sca_Delta_H = 1.0004;
  double sca_S = 1.0029;
  double ZPE = 0.;
  double Delta_enthalpy = 0.;
  double minus_T_entropy = 0.;
  double thermal = 0.;
  double tmpval1 = 0., tmpval2 = 0.;
  char tmpstr[BUFSIZ] = "";
  char noappname[BUFSIZ] = "";
  char c = '\0';
  Bool isinteractive = False;

  /*  read arguments  */
  if (argc == 2)
  {
    if (! (strcmp(argv[1], "--help") \
           && strcmp(argv[1], "-h") \
           && strcmp(argv[1], "/?")))
    {
      /*  help  */
      puts("Usage: GauFreqScaler FileName Scaler_for_ZPE Scaler_for_Delta_H Scaler_for_S Temperature/K Pressure/bar");
      puts("FileName Can be xxx.chk/fch/fchk");
      puts("If no argument is provided in the command line,");
      puts("then you can input those values interactively,");
      puts("In this mode, if you directly press ENTER at any time,");
      puts("Then the default value shown below will be used.");
      puts("In command line argument(s) mode,");
      puts("if not enough scalers are provided in the command line,");
      puts("Then those in 10.1021/jp073974n for B3LYP/6-31G* will be applied.");
      puts("If either temperature or pressure is not provided,");
      puts("298.15 K and 1.0 bar will be used.");
      puts("Note: You need to properly set your PATH environmant variable so that");
      puts("it contains the directory containing your Gaussian executable.");
      return 0;
    }
  }
  if (argc > 1)
  {
    strcpy(fname, argv[1]);
    if (argc >= 5)
    {
      sscanf(argv[2], "%lf", & sca_ZPE);
      sscanf(argv[3], "%lf", & sca_Delta_H);
      sscanf(argv[4], "%lf", & sca_S);
      if (argc == 7)
      {
        sscanf(argv[5], "%lf", & temp);
        sscanf(argv[6], "%lf", & pres);
      }
    }
  }
  else
  {
    isinteractive = True;
    puts("Input chk/fch/fchk file name:");
    scanf("%[^\n]%c", fname, & c);
    puts("Input the scaler for Zero-Point Energy:");
    fgets(tmpstr, BUFSIZ, stdin);
    while (strlen(tmpstr) && tmpstr[strlen(tmpstr) - 1] == '\n')
      tmpstr[strlen(tmpstr) - 1] = '\0';
    if (strcmp(tmpstr, ""))
      sscanf(tmpstr, "%lf", & sca_ZPE);
    puts("Input the scaler for Enthalpy change caused by non-zero temerature:");
    fgets(tmpstr, BUFSIZ, stdin);
    while (strlen(tmpstr) && tmpstr[strlen(tmpstr) - 1] == '\n')
      tmpstr[strlen(tmpstr) - 1] = '\0';
    if (strcmp(tmpstr, ""))
      sscanf(tmpstr, "%lf", & sca_Delta_H);
    puts("Input the scaler for Entropy:");
    fgets(tmpstr, BUFSIZ, stdin);
    while (strlen(tmpstr) && tmpstr[strlen(tmpstr) - 1] == '\n')
      tmpstr[strlen(tmpstr) - 1] = '\0';
    if (strcmp(tmpstr, ""))
      sscanf(tmpstr, "%lf", & sca_S);
    puts("Input the temperature in unit K:");
    fgets(tmpstr, BUFSIZ, stdin);
    while (strlen(tmpstr) && tmpstr[strlen(tmpstr) - 1] == '\n')
      tmpstr[strlen(tmpstr) - 1] = '\0';
    if (strcmp(tmpstr, ""))
      sscanf(tmpstr, "%lf", & temp);
    puts("Input the pressure in unit bar:");
    fgets(tmpstr, BUFSIZ, stdin);
    while (strlen(tmpstr) && tmpstr[strlen(tmpstr) - 1] == '\n')
      tmpstr[strlen(tmpstr) - 1] = '\0';
    if (strcmp(tmpstr, ""))
      sscanf(tmpstr, "%lf", & pres);
  }

  /*  check file  */
  fl = fopen(fname, "rb");
  if (! fl)
  {
    puts("Error! Cannot open this file!");
    puts("\nPress ENTER to exit...");
    while ((cont =  getchar()) != '\n' && cont != EOF)
      ;
    exit(ERR_FILE_NOT_FOUND);
  }
  fclose(fl);
  fl = NULL;

  /*  formchk, optional  */
  appname = strrchr(fname, '.');
  fnleng = strlen(fname) - strlen(appname);
  if (! strcmp(appname, ".chk"))
  {
    puts("Formatting chk...");
    strncpy(fchname, fname, fnleng);
    fchname[fnleng] = '\0';
    strcat(fchname, ".fch");
    strcpy(comm, "formchk ");
    strcat(comm, fname);
    strcat(comm, " ");
    strcat(comm, fchname);
    system(comm);
  }
  else
  {
    strcpy(fchname, fname);
    if ((strcmp(appname, ".fch")) && (strcmp(appname, ".fch")))
    {
      puts("Error! Unknown file type! Please use chk/fch/fchk as extension.");
      puts("\nPress ENTER to exit...");
      while ((cont =  getchar()) != '\n' && cont != EOF)
        ;
      exit(UNKNOWN_FILE_TYPE);
    }
  }
  strncpy(noappname, fchname, fnleng);
  noappname[fnleng] = '\0';

  puts("Calculating... please wait");

  /*  for ZPE  */
  strcpy(comm, "freqchk ");
  strcat(comm, fchname);
  strcat(comm, " N");
  sprintf(tmpstr, " %6.2lf", temp);
  strcat(comm, tmpstr);
  sprintf(tmpstr, " %7.5lf", pres);
  strcat(comm, tmpstr);
  sprintf(tmpstr, " %6.4lf", sca_ZPE);
  strcat(comm, tmpstr);
  strcat(comm, " Y N > ");
  strcpy(tmpstr, noappname);
  strcat(tmpstr, "_ZPE.txt");
  strcat(comm, tmpstr);
  system(comm);

  fl = fopen(tmpstr, "r");
  while (! feof(fl))
  {
    fgets(line, BUFSIZ, fl);
    if (strstr(line, "Zero-point correction="))
    {
      sscanf(line, " Zero-point correction= %lf", & ZPE);
      break;
    }
  }
  fclose(fl);
  fl = NULL;
  # ifdef DEL_TMP_OUT_FILES
    remove(tmpstr);
  # endif

  /*  for Delta_H  */
  strcpy(comm, "freqchk ");
  strcat(comm, fchname);
  strcat(comm, " N");
  sprintf(tmpstr, " %6.2lf", temp);
  strcat(comm, tmpstr);
  sprintf(tmpstr, " %7.5lf", pres);
  strcat(comm, tmpstr);
  sprintf(tmpstr, " %6.4lf", sca_Delta_H);
  strcat(comm, tmpstr);
  strcat(comm, " Y N > ");
  strcpy(tmpstr, noappname);
  strcat(tmpstr, "_Delta_H.txt");
  strcat(comm, tmpstr);
  system(comm);

  fl = fopen(tmpstr, "r");
  while (! feof(fl))
  {
    fgets(line, BUFSIZ, fl);
    if (strstr(line, "Zero-point correction="))
    {
      sscanf(line, " Zero-point correction= %lf", & tmpval1);
      break;
    }
  }
  while (! feof(fl))
  {
    fgets(line, BUFSIZ, fl);
    if (strstr(line, "Thermal correction to Enthalpy="))
    {
      sscanf(line, " Thermal correction to Enthalpy= %lf", & tmpval2);
      break;
    }
  }
  Delta_enthalpy = tmpval2 - tmpval1;
  fclose(fl);
  fl = NULL;
  # ifdef DEL_TMP_OUT_FILES
    remove(tmpstr);
  # endif

  /*  for S  */
  strcpy(comm, "freqchk ");
  strcat(comm, fchname);
  strcat(comm, " N");
  sprintf(tmpstr, " %6.2lf", temp);
  strcat(comm, tmpstr);
  sprintf(tmpstr, " %7.5lf", pres);
  strcat(comm, tmpstr);
  sprintf(tmpstr, " %6.4lf", sca_S);
  strcat(comm, tmpstr);
  strcat(comm, " Y N > ");
  strcpy(tmpstr, noappname);
  strcat(tmpstr, "_S.txt");
  strcat(comm, tmpstr);
  system(comm);
  fl = fopen(tmpstr, "r");
  while (! feof(fl))
  {
    fgets(line, BUFSIZ, fl);
    if (strstr(line, "Thermal correction to Enthalpy="))
    {
      sscanf(line, " Thermal correction to Enthalpy= %lf", & tmpval1);
      break;
    }
  }
  while (! feof(fl))
  {
    fgets(line, BUFSIZ, fl);
    if (strstr(line, "Thermal correction to Gibbs Free Energy="))
    {
      sscanf(line, " Thermal correction to Gibbs Free Energy= %lf", & tmpval2);
      break;
    }
  }
  minus_T_entropy = tmpval2 - tmpval1;
  fclose(fl);
  fl = NULL;
  # ifdef DEL_TMP_OUT_FILES
    remove(tmpstr);
  # endif

  /*  print result  */
  thermal = ZPE + Delta_enthalpy + minus_T_entropy;
  puts("Scalers      ZPE     Delta H     S");
  printf("           %6.4lf    %6.4lf    %6.4lf\n", sca_ZPE, sca_Delta_H, sca_S);
  puts("Thermal data after scaled:");
  printf("ZPE           : %8.2lf kJ/mol\n", ZPE * HartreeTokJpermol);
  printf("Delta Enthalpy: %8.2lf kJ/mol\n", Delta_enthalpy * HartreeTokJpermol);
  printf("Entropy       : %8.2lf  J/(mol*K)\n", minus_T_entropy * HartreeTokJpermol *(-1000) / temp);
  printf("Total Thermal Energy without electron energy:\n");
  printf("    %9.6lf Hartree        %8.2lf kJ/mol\n", thermal, thermal * HartreeTokJpermol);

  /*  pause  */
  if (isinteractive)
  {
    puts("\nPress ENTER to exit...");
    while ((cont =  getchar()) != '\n' && cont != EOF)
      ;
  }

  return 0;
}

