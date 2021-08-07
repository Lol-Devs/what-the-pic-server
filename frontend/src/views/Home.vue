<template>
  <div>
    
   <div class="flex justify-center"> <h1 class="flex text-center text-5xl font-bold mt-10 mb-4">
      WHAT THE P<div class="animate-bounce">i</div>C
    </h1></div>

    <p class="text-center text-gray-500 mb-12">Klick a camaera to see more information</p>

    <card v-for="camera in cameras" :id="camera.id" :status="0" :key="camera.id"></card>


  </div>
</template>

<script>
import Card from '../components/Listcard.vue'
import Service from "../cameras.js";
export default {
  components:{
    Card
  },
  name: "List",
  created() {},
  mounted() {
    this.getCameras();
  },
  data() {
    return {
      cameras: []
    };
  },
  props: {},
  methods: {
    getCameras() {

      Service.getAllCameras()
        .then(cameras => this.cameras = cameras)

    },
  },
};
</script>