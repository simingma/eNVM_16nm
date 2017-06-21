This is the git repository for 16nm eNVM testing
after experiencing the HardDrive crisis (June 17).

Files organization:

Google drive SYNCH'ed with my MAC(my drive folder): store (the latest version of) everything! Two copies backups, BUT SYNCH'ed as long as there's internet!
/Siming_16nm
    /Data: chip testing data
	/working: ongoing testing always writes data file to this folder
	/**Chip**: all the folders for specific set of testing data
    /Testing_Code: the latest version of c++ code, header files, libraries, etc. for testing
    /Scan_files: the latest version of NIDAQ scan files
    /Scripts: the latest version of python scripts for analysing and ploting data
    /Plots: plots organized into subfolders
	/**Chip**

github repository (local and remote): version control frequently editing source code (total size not very big, github cannot be used as backup)
/Siming_16nm
    /Testing_Code: version controled, I can experiment with incremental tests even using one file for one chip, while not worrying about loosing intermediate code states as long as I commit to git after each modification and push to github periodically
    /Scan_files: version controlled, current and future additional scan files are version controlled and the evolution history is recorded 
    /Scripts: version controlled, data analysing and ploting scripts

The testing computer: only store (and synch'ed with google drive) those necessary realtime testing files, to minimize the number of synch'ed devices (to avoid unintentional data corruption to be accidentally synch'ed) and reduce storage usage 
/Siming_16nm
    /Testing_Code: latest version
    /Scan_files: latest version
    /Data
	/Working: only store the data generated by the currently running testing. This is a temporary folder, data in it will be later sorted into other folders and stored in my MAC and Google drive. This testing computer does not synch those sorted permanent folders
