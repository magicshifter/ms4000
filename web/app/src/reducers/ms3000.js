import {
  IMAGE_UPLOAD_REQUEST_START,
  IMAGE_UPLOAD_REQUEST_SUCCESS,
  IMAGE_UPLOAD_REQUEST_FAIL,

  IMAGE_DOWNLOAD_REQUEST_START,
  IMAGE_DOWNLOAD_REQUEST_SUCCESS,
  IMAGE_DOWNLOAD_REQUEST_FAIL,

  CONFIG_UPDATE,

  CONFIG_DOWNLOAD_REQUEST_START,
  CONFIG_DOWNLOAD_REQUEST_SUCCESS,
  CONFIG_DOWNLOAD_REQUEST_FAIL,

  CONFIG_UPLOAD_REQUEST_START,
  CONFIG_UPLOAD_REQUEST_SUCCESS,
  CONFIG_UPLOAD_REQUEST_FAIL
} from '../actions/ms3000'

import {FILESYSTEM_REQUEST_START, FILESYSTEM_REQUEST_SUCCESS} from "../actions/filesystem";



const DEFAULT_STATE = {
  isFetching: false,
  shifterState: {},
  //host: 'http://ms3000-288f.local',
  host: 'http://192.168.4.1',

  isUploading: false,
  uploadError: null,

  isDownloading: false,
  downloadError: null,

  isConfigUploading: false,
  configUploadError: null,

  isConfigDownloading: false,
  configDownloadError: null,
}



const ms3000 = (state = DEFAULT_STATE, action) => {
  switch (action.type) {
    case CONFIG_UPDATE:
      return {
        ...state,
        shifterState: action.shifterState,
      };


    case CONFIG_UPLOAD_REQUEST_START:
      return {
        ...state,
        isConfigUploading: true,
        configUploadError: null,
      }

    case CONFIG_UPLOAD_REQUEST_SUCCESS:
      return {
        ...state,
        isConfigUploading: false,
        configUploadError: null
      }

    case CONFIG_UPLOAD_REQUEST_FAIL:

      return {
        ...state,
        files: null,
        isConfigUploading: false,
        configUploadError: action.error
      }


    case CONFIG_DOWNLOAD_REQUEST_START:
      return {
        ...state,
        isConfigDownloading: true,
        configDownloadError: null,
      }

    case CONFIG_DOWNLOAD_REQUEST_SUCCESS:
      return {
        ...state,
        isConfigDownloading: false,
        configDownloadError: null
      }

    case CONFIG_DOWNLOAD_REQUEST_FAIL:
      return {
        ...state,
        isConfigDownloading: false,
        configDownloadError: action.error
      }



    case IMAGE_UPLOAD_REQUEST_START:
      console.log("IMAGE_UPLOAD_REQUEST_START", IMAGE_UPLOAD_REQUEST_START)
      return {
        ...state,
        isUploading: true,
        uploadError: null,
      }

    case IMAGE_UPLOAD_REQUEST_SUCCESS:
      console.log("IMAGE_UPLOAD_REQUEST_SUCCESS", IMAGE_UPLOAD_REQUEST_SUCCESS)
      return {
        ...state,
        isUploading: false,
        uploadError: null
      }

    case IMAGE_UPLOAD_REQUEST_FAIL:
      console.log("IMAGE_UPLOAD_REQUEST_FAIL", IMAGE_UPLOAD_REQUEST_FAIL, action)
      //alert(action)
      return {
        ...state,
        isUploading: false,
        uploadError: action.error
      }


    case IMAGE_DOWNLOAD_REQUEST_START:
      return {
        ...state,
        isDownloading: true,
        downloadError: null,
      }

    case IMAGE_DOWNLOAD_REQUEST_SUCCESS:
      return {
        ...state,
        isDownloading: false,
        downloadError: null
      }

    case IMAGE_DOWNLOAD_REQUEST_FAIL:
      return {
        ...state,
        isDownloading: false,
        downloadError: action.error
      }

    default:
      return state
  }
}



export default ms3000
