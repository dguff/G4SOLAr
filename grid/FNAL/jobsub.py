#!/usr/bin/env python    

##################################################                                                                                                                                                                                                                              
# G4SoL grid job submitter                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
# Made to work with parameter_uboone_template_grid.dat                                                                                                                                                                                                                                 
# 2020.01 -- A.Navrer-Agasson                                                                                                                                                                                                                                                   
##################################################                                                                                                                                                                                                                              
import os, optparse, random, shutil, tarfile, sys
import subprocess, string
import time


PWD = os.getenv("PWD")

##################################################                                                                                                                                                                                                                              
# Job Defaults                                                                                                                                                                                                                                                                  
################################################## 
USER                  = os.getenv("USER") 
DO_TAR                = True                                                                                                                                                                                                                     
NEVTS                 = 50
N_JOBS                = 100
RUN_NUMBER            = 20221017
PROJECT               = "g4solar"
OUTDIR                = "/pnfs/dune/scratch/users/{USER}/{PROJECT}/run{RUN_NUMBER}/files/".format( USER = os.getenv("USER"),  PROJECT = PROJECT, RUN_NUMBER = RUN_NUMBER)
LOGDIR                = "/pnfs/dune/scratch/users/{USER}/{PROJECT}/run{RUN_NUMBER}/log".format( USER = os.getenv("USER"), PROJECT = PROJECT, RUN_NUMBER = RUN_NUMBER)
TEMPLATE              = "template_g4solar_electrons.mac".format(USER = os.getenv("USER"))
FILETAG               = ""
TARFILE_NAME          = "local_install_g4solar.tar.gz"

ENERGY                = 10 #MeV
XSTART                = 0
YSTART                = 0
ZSTART                = 0


def get_options():
    parser       = optparse.OptionParser(usage="usage: %prog [options]")
    grid_group   = optparse.OptionGroup(parser, "Grid Options")

    grid_group.add_option("--make_tar", default = DO_TAR, help = "Make new tarfile. Default = False")
    grid_group.add_option("--outdir", default = OUTDIR, help = "Output flux histograms location. Default = %default.")
    grid_group.add_option("--logdir", default = LOGDIR, help = "Output log file location. Default = %default.")
    grid_group.add_option("--n_jobs", default = N_JOBS, type=int, help = "Number of g4numi jobs. Default = %default.")
    grid_group.add_option("--run_number", default = RUN_NUMBER, type=int, help = "Tag on the end of outfiles. Doubles as random # seed. Default = %default.")
  
    g4solar_group    = optparse.OptionGroup(parser, "BdNMC Options")
    g4solar_group.add_option('--template', default = TEMPLATE, help='Specify template parameter file. Default = template_g4solar_electrons.mac')
    g4solar_group.add_option('--nevts', default = NEVTS, type=int, help='Specify number of events to generate. Default = 500.')
    g4solar_group.add_option('--energy', default = ENERGY, type=float, help='Specify particle energy. Default = 10 MeV')
    g4solar_group.add_option('--xstart', default = XSTART, type=float, help='Specify x starting position. Default = 0.')
    g4solar_group.add_option('--ystart', default = YSTART, type=float, help='Specify y starting position. Default = 0.')
    g4solar_group.add_option('--zstart', default = ZSTART, type=float, help='Specify z starting position. Default = 0.')
  
    parser.add_option_group(grid_group)
    parser.add_option_group(g4solar_group)

    options, remainder = parser.parse_args()

    print("\nPrinting config options to run with: \n")
    print('run',                       options.run_number)

    return options

def make_parfile(options):
  template_filename = options.template
  template_string   = open(template_filename, 'r').read()
  template          = string.Template(template_string)

  macro_string = template.safe_substitute(
      {
        'nevts'  :    str(options.nevts),
        'energy' :    str(options.energy),
        'xstart' :    str(options.xstart),
        'ystart' :    str(options.ystart),
        'zstart' :    str(options.zstart),
      }
    )

  macro_name = "parameter_g4solar_grid.mac"
  macro = open(macro_name, "w") 
  macro.write(macro_string)
  macro.close()

  return macro_name

def make_tarfile(output_filename):
    os.listdir(".")
    tar = tarfile.open(output_filename, "w:gz")
    tar.add("parameter_g4solar_grid.mac")
    tar.add("../setup_g4solar.sh", arcname="setup_g4solar.sh")
    for i in os.listdir("../install/"):
      tar.add("../install/"+i, arcname = i)
    #tar.add("../install/solar_sim", arcname="solar_sim")
    tar.close()



def main():

    print("Job submission")

    options       = get_options()
    parfile       = make_parfile(options)
    template      = options.template
    make_tar      = options.make_tar
    

    # Create a run number directory                                                                                                                                                                                                                                               
    RUNDIR = "/pnfs/dune/scratch/users/{USER}/{PROJECT}/run_{RUN_NUMBER}/".format( USER = os.getenv("USER"), PROJECT = PROJECT, RUN_NUMBER = options.run_number)
    print(RUNDIR)

    if os.path.isdir(RUNDIR) == False:
        print(RUNDIR, " directory doen't exist, so creating...\n")
        os.makedirs(RUNDIR)

    # Create a output file directory                                                                                                                                                                                                                                              
    OUTDIR = "/pnfs/dune/scratch/users/{USER}/{PROJECT}/run_{RUN_NUMBER}/files/".format( USER = os.getenv("USER"), PROJECT = PROJECT, RUN_NUMBER = options.run_number)

    if os.path.isdir(OUTDIR) == False:
        print(OUTDIR, " directory doen't exist, so creating...\n")
        os.makedirs(OUTDIR)

    # Create a log file directory                                                                                                                                                                                                                                                 
    LOGDIR = "/pnfs/dune/scratch/users/{USER}/{PROJECT}/run_{RUN_NUMBER}/log/".format( USER = os.getenv("USER"), PROJECT = PROJECT, RUN_NUMBER = options.run_number)

    if os.path.isdir(LOGDIR) == False:
        print(LOGDIR, " directory doen't exist, so creating...\n")
        os.makedirs(LOGDIR)
    
    # Create a cache file directory  
    CACHE_PNFS_AREA = "/pnfs/dune/scratch/users/{USER}/{PROJECT}/run_{RUN_NUMBER}/CACHE/".format(USER = os.getenv("USER"), PROJECT = PROJECT, RUN_NUMBER = options.run_number)

    if os.path.isdir(CACHE_PNFS_AREA) == False:
      print(CACHE_PNFS_AREA, " directory doen't exist, so creating...\n")
      os.makedirs(CACHE_PNFS_AREA)

    # scratch /pnfs area from which to send tarfile to grid                                                                                                                                                                                                                       
    cache_folder = CACHE_PNFS_AREA + str(random.randint(10000,99999)) + "/"
    os.makedirs(cache_folder)

    if(make_tar):
      print("\nTarring up local area...")
      make_tarfile(TARFILE_NAME)  

    #always copy jobfile and tarball to cache area
    shutil.copy(TARFILE_NAME,    cache_folder)
    shutil.copy("/dune/app/users/anavrera/SoLAr/G4SOLAr/grid/g4solar_job.sh", cache_folder)

    print("\nTarball of local area:", cache_folder + TARFILE_NAME)

    logfile = LOGDIR + "/g4solar_{RUN}_{TIME}_\$PROCESS.log".format(RUN = options.run_number, TIME = time.strftime("%Y%m%d-%H%M%S"))

    print("\nOutput logfile(s):",logfile)

    submit_command = ("jobsub_submit {GRID} {MEMORY} {DISK} -N {NJOBS} -d G4SOLAR {OUTDIR} "
      "-G dune "
      "-e RUN={RUN} "
      "-e NEVTS={NEVTS} "
      "-f {TARFILE} "
      "-L {LOGFILE} "
      "file://{CACHE}/g4solar_job.sh".format(
      GRID       = ("--OS=SL7 -g "
                    "--resource-provides=usage_model=DEDICATED,OPPORTUNISTIC,OFFSITE "
                    "--role=Analysis "
                    "--expected-lifetime=2h "),
      DISK       = "--disk 0.5GB",
      MEMORY     = "--memory 0.5GB ",
      NJOBS      = options.n_jobs,
      OUTDIR     = OUTDIR,
      RUN        = options.run_number,
      NEVTS      = options.nevts,
      TARFILE    = cache_folder + TARFILE_NAME,
      LOGFILE    = logfile,
      CACHE      = cache_folder)
  )

  #Ship it                                                                                                                                                                                                                                                                      
    print("\nSubmitting to grid:\n"+submit_command+"\n")
    status = subprocess.call(submit_command, shell=True)

if __name__ == "__main__":
    sys.exit(main())
    #ptions = get_options()
    #parfile = make_parfile(options)