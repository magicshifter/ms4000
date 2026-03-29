import protobuf from 'protobufjs'
//import ms3kg from '../proto-generated/ms3kg'


var xxx = {};

//console.warn("SDFsdfsdf", ms3kg)


//console.log("test hack", protobufs, protobufs.test)

export function getProtocolBuffersPromise() {
  var promise = new Promise(function(resolve, reject) {
    protobuf.load(process.env.PUBLIC_URL + "MS4.proto", function (err, root) {
      if (err) {
        reject(Error(err));
      }
      else {
        // Obtain a message type
        xxx.MS3KG = root.lookupType("MS3KG");
        xxx.root = root

        console.log("stage2", xxx)
        resolve();
      }
    })
  });

  return promise
}




export default xxx;
