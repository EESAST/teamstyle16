import hashlib

def GetFileMd5(strFile):
	    file_ = None
	    bRet = False
	    strMd5 = ""
	    
	    try:
	        file_ = open(strFile, "rb")
	        md5 = hashlib.md5()
	        strRead = ""
	        
	        while True:
	            strRead = file_.read(8096)
	            if not strRead:
	                break
	            md5.update(strRead)
	        #read file finish
	        bRet = True
	        strMd5 = md5.hexdigest()
	    except:
	        bRet = False
	    finally:
	        if file_:
	            file_.close()

	    return strMd5

if __name__ == '__main__':
	strPath = raw_input("please input get md5 file:")
	print GetFileMd5(strPath)