# eefs
NASA's EFFS file system application
Here ,We just implementation standalone_driver , and the specific initialization process is shown in the eefs_userInit.c file.
Code structure is：


![image](https://user-images.githubusercontent.com/63274663/217208728-abf08a99-dc49-4f3e-b412-daf7fb090868.png)

Example of program initialization：

if(setup_eefs() > 0){
 eefs_test();
}

The implemented interface functions：


![image](https://user-images.githubusercontent.com/63274663/217209512-43af81a0-b334-4a48-b2f2-11cc0568ef1c.png)

Enjoy it！！！

