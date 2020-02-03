# The Watchers : Work in Progress    
This is just a driver/client application that will monitor IRP_MJ_DEVICE_CONTROL to see how your target is communicating with the kernel. This is a nice way to escalate privileges if you have the ability to interact with the driver as a low privileged user.    
This is helpful because it will allow you to see what the most commonly used IOCTL's are. Pretty much everything about the IRP packet. This includes, but not limited to, the following:    


1. The IOCTL code to interact with the kernel    
2. The input buffer, as well as the size    
3. The output buffer, as well as the size    
4. The output    
5. Everything else    

This can be modified to do whatever you want, but to be clear, this *isn't* malware. This just aids in finding potential vulnerabilities when you're hunting in the upside down of Windows.    
In an elevated command prompt, you would need to issue the following commands to be able to use this driver and then reboot:    
    
    bcdedit /set /nointegritychecks on    
    bcdedit /set testsigning on    
   
Then to re-enable them, set them to off. You'll know if you did it correctly because on the lower right of the screen, you'll see a Test Mode watermark.   

So far, it's coming together:    
![pic](https://github.com/cybersurfers/Hunting-Windows/blob/master/WindowsKernel/Images/updated.PNG)    

The address displayed in the picture is the adress to the function(s) I am hooking. This can also be used to display the address of the dispatch table that houses all of the IOCTL's    

# To-Dos    


~~1. The client will be able to specify what driver to hook. Not all will be able to get hooked.~~     
2. Might add the ability to make it part of the context menu when you right click    
3. Do a replay type functionality to fuzz    
4. Pretend I know what I'm doing
5. Parse IRP_MJ_DEVICE_CONTROL to get alll the information being passed back and forth
