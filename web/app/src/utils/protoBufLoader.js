import protobuf from 'protobufjs'


var xxx = {};
var protocolBuffersPromise = null;

//console.log("test hack", protobufs, protobufs.test)

export function getProtocolBuffersPromise() {
  if (xxx.MS4) {
    return Promise.resolve(xxx)
  }

  if (!protocolBuffersPromise) {
    protocolBuffersPromise = new Promise(function(resolve, reject) {
      protobuf.load("/MS4.proto", function (err, root) {
        if (err) {
          protocolBuffersPromise = null
          reject(Error(err));
        }
        else {
          // Obtain a message type
          xxx.MS4 = root.lookupType("MS4");
          xxx.root = root

          console.log("stage2", xxx)
          resolve(xxx);
        }
      })
    });
  }

  return protocolBuffersPromise
}




export default xxx;
