from __future__ import print_function
from googleapiclient.discovery import build
from httplib2 import Http
from oauth2client import file, client, tools

#import cryption

def uploadToGoogleDrive():
    try :
        import argparse
        flags = argparse.ArgumentParser(parents=[tools.argparser]).parse_args()
    except ImportError:
        flags = None

    SCOPES = 'https://www.googleapis.com/auth/drive.file'
    store = file.Storage('storage.json')
    creds = store.get()

    if not creds or creds.invalid:
        print("make new storage data file ")
        #oauth json파일
        flow = client.flow_from_clientsecrets('C:\\test\\client_secret_828580371975-fvfcc5vutfr2cb3h7tckt2kgimh8ltub.apps.googleusercontent.com.json', SCOPES)
        creds = tools.run_flow(flow, store, flags) if flags else tools.run(flow, store)

    DRIVE = build('drive', 'v3', http=creds.authorize(Http()))

    FILES = (
        ('C:/Users/조영욱/source/repos/2021-1-CECD4-REC-OVER-5-encrypt/vs/test_tv/encrypted.bin'), #업로드할파일
    )

    folder_id = '15ND8PEYFwXOttX8NaltOQHtFEFP9kIul' #구글드라이브 폴더이름

    for file_title in FILES :

        file_name = file_title
        metadata = {'name': 'encryptedFile', #이 이름으로 업로드
                    'parents': [folder_id],
                    'mimeType': None
                    }

        res = DRIVE.files().create(body=metadata, media_body=file_name).execute()

        if res:
            print('Uploaded "%s" (%s)' % (file_name, res['mimeType']))
    print("구글드라이브 업로드 완료")
