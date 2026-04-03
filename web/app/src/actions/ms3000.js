import {fetch as legacyFetch } from "../utils/http";
import { trimExtension } from '../utils/files'
import pb from '../utils/protoBufLoader'
import MagicBitmap from "../ms3000/MagicBitmap";

import { pixelEditorChangeImage } from './pixelEditor'
import { filesystemFakeAddFile } from './filesystem'



export const IMAGE_UPLOAD_REQUEST_START = "IMAGE_UPLOAD_REQUEST_START"
export const IMAGE_UPLOAD_REQUEST_SUCCESS = "IMAGE_UPLOAD_REQUEST_SUCCESS"
export const IMAGE_UPLOAD_REQUEST_FAIL = "IMAGE_UPLOAD_REQUEST_FAIL"


const imageUploadStart = (name, magicBitmap) => ({
  type: IMAGE_UPLOAD_REQUEST_START,
  name,
  magicBitmap
})
const imageUploadSuccess = () => ({
  type: IMAGE_UPLOAD_REQUEST_SUCCESS,
})
const imageUploadFail = (error) => ({
  type: IMAGE_UPLOAD_REQUEST_FAIL,
  error
})

function uploadError(dispatch, error) {
  const err = "Image Upload Failed! " + error
  console.log(err)
  //alert(err)
  dispatch(imageUploadFail(error))
}

export const imageUpload = () => (dispatch, getState) => {
  dispatch(imageUploadStart())

  const state = getState()
  const { host } = state.ms3000
  const { width, height, frames, imageName } = state.pixelEditor.present

// TODO: implement arrayu!!!
  try {
    const mb = new MagicBitmap('bitmap', 24, width, height, frames, [999])
    const blob = mb.toBlob()
    const url = host + '/upload';
    const fileName = trimExtension(imageName) + '.magicBitmap'
    const formData = new window.FormData();
    formData.append('uploadFile', blob, fileName);

    const request = new window.XMLHttpRequest();
    request.onload =
      (e) => {
        e.preventDefault()
        if (request.status === 200) {
          console.log("uploaded the bitmap :) ")
          dispatch(imageUploadSuccess())
          dispatch(filesystemFakeAddFile(fileName))
        }
        else {
          uploadError('Request Failed: ' + request.status)
        }
      }

    request.timeout = 10000;
    request.ontimeout =
      (e) => {
        e.preventDefault()
        console.log('ontimeout', e)
        uploadError(dispatch, 'Timeout')
        return false
      }
    request.onerror =
      (e) => {
        e.preventDefault()
        console.log('onerror', e)
        uploadError(dispatch, e)
        return false
      }
    request.onabort =
      (e) => {
        e.preventDefault()
        console.log('onabort', e)
        //alert(e)
        uploadError(dispatch, 'Aborted')
        return false
      }

    //console.log("postin", url)
    request.open('POST', url);
    request.send(formData);
  }
  catch (e) {
    uploadError(dispatch, 'Exception: ' + e.toString())
  }
}


export const IMAGE_DOWNLOAD_REQUEST_START = "IMAGE_DOWNLOAD_REQUEST_START"
export const IMAGE_DOWNLOAD_REQUEST_SUCCESS = "IMAGE_DOWNLOAD_REQUEST_SUCCESS"
export const IMAGE_DOWNLOAD_REQUEST_FAIL = "IMAGE_DOWNLOAD_REQUEST_FAIL"


const imageDownloadStart = (name) => ({
  type: IMAGE_DOWNLOAD_REQUEST_START,
  name,
})
const imageDownloadSuccess = () => ({
  type: IMAGE_DOWNLOAD_REQUEST_SUCCESS,
})
const imageDownloadFail = (error) => ({
  type: IMAGE_DOWNLOAD_REQUEST_FAIL,
  error
})

export const imageDownload = (name) => (dispatch, getState) => {
  dispatch(imageDownloadStart())

  console.log("started download", name)

  const state = getState()
  const { host } = state.ms3000
  const url = host + '/download?file=' + name;

  const imageName = trimExtension(name)


  fetch(url)
    .then(data => data.arrayBuffer())
    .then((arrayBufffer) => {
      const mb = MagicBitmap.fromArrayBuffer(arrayBufffer)
      const image = mb.toImage()

      dispatch(imageDownloadSuccess())
      dispatch(pixelEditorChangeImage(image, 0, imageName))
    })
    .catch(error => {
      console.error('Error:', error)
      dispatch(imageDownloadFail(error))
    });
}


export const CONFIG_UPLOAD_REQUEST_START = "CONFIG_UPLOAD_REQUEST_START"
export const CONFIG_UPLOAD_REQUEST_SUCCESS = "CONFIG_UPLOAD_REQUEST_SUCCESS"
export const CONFIG_UPLOAD_REQUEST_FAIL = "CONFIG_UPLOAD_REQUEST_FAIL"

const configUploadStart = (shifterState) => ({
  type: CONFIG_UPLOAD_REQUEST_START,
  shifterState
})

const configUploadSuccess = () => ({
  type: CONFIG_UPLOAD_REQUEST_SUCCESS,
})

const configUploadFail = (error) => ({
  type: CONFIG_UPLOAD_REQUEST_FAIL,
  error
})

function stringToArray(bufferString) {
  var array = new Uint8Array(new ArrayBuffer(bufferString.length));

  for (var i = 0; i < bufferString.length; i++) {
    array[i] = bufferString.charCodeAt(i);
  }
  return array
}

export const configUpload = () => (dispatch, getState) => {
  dispatch(configUploadStart())

  const state = getState()
  const { host } = state.ms3000

  console.log("configUpload", state)

  const testObj = state.ms3000.shifterState
  var check = pb.MS4.verify(testObj);

  if (check) {
    console.warn("buffer verify error, corrupt?",  check)
    //alert("buffer verify error, corrupt? " + check)
  }

  const bufferU8 = pb.MS4.encode(testObj).finish()
  const funkyStr = String.fromCharCode.apply(null, bufferU8)
  const b64encoded = btoa(funkyStr);

  fetch(host + '/protobuf?myArg=' + b64encoded, { method: 'POST'})
    .then(() => {
      dispatch(configUploadSuccess())
    })
    .catch(error => {
      dispatch(configUploadFail(error))
    });

  console.log(check, bufferU8)
}


export const CONFIG_DOWNLOAD_REQUEST_START = "CONFIG_DOWNLOAD_REQUEST_START"
export const CONFIG_DOWNLOAD_REQUEST_SUCCESS = "CONFIG_DOWNLOAD_REQUEST_SUCCESS"
export const CONFIG_DOWNLOAD_REQUEST_FAIL = "CONFIG_DOWNLOAD_REQUEST_FAIL"


const configDownloadStart = () => ({
  type: CONFIG_DOWNLOAD_REQUEST_START,
})

const configDownloadSuccess = () => ({
  type: CONFIG_DOWNLOAD_REQUEST_SUCCESS,
})

const configDownloadFail = (error) => ({
  type: CONFIG_DOWNLOAD_REQUEST_FAIL,
  error
})

export const configDownload = () => (dispatch, getState) => {
  dispatch(configDownloadStart())

  const state = getState()
  const { host } = state.ms3000

  fetch(host + '/protobuf')
    .then(data => data.text())
    .then(text => {
      var decoded = atob(text)
      console.log("b64 decoded", decoded)
      var u8a = stringToArray(decoded);
      //dumpU8(u8a)
      console.log("u8", u8a)

      try {
        const shifterState = pb.MS4.decode(u8a);

        var object = pb.MS4.toObject(shifterState, {
          longs: undefined,
          enums: undefined,
          bytes: undefined,
        });

        dispatch(configDownloadSuccess())
        dispatch(configUpdate(object))
      }
      catch (ex) {
        dispatch(configDownloadFail("shifterstate fetch decode error: " + ex))
      }
    })
    .catch(error => {
      console.error('Error:', error)
      dispatch(configDownloadFail(error))
    });
}


export const CONFIG_UPDATE = 'CONFIG_UPDATE'

export const configUpdate = (shifterState) => ({
  type: CONFIG_UPDATE,
  shifterState,
})
