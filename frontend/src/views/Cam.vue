<template>
  <div v-if="camera">

    <router-link to="/"><p class="text-gray-400 mt-2 ml-2">
      BACK TO LIST
    </p></router-link>

    <h1 class="text-3xl font-bold text-center my-5">
      Camera No. {{camera.id}}
    </h1>

    <div class="flex flex-col items-center">

      <div>
        <p class="text-lg font-bold mb-4 ">
          Settings
        </p>
        <div class="mb-4"><label class="mr-4 text-sm" for="inter">interval (ms)</label>
      <input :value="camera.interval" type="number" min="0" placeholder="Milliseconds" class="border-b-2 focus:outline-none" name="" id="inter"></div>
      <div class="flex justify-start items-center">
        <label class="mr-4 text-sm" for="qual">quality</label>
      <input type="range" class="" :value="camera.quality" step="1" min="10" max="63" name="" id="qual">
      <label class="ml-3 text-gray-400" for="qual">{{val}} </label>
      </div>
      </div>

    </div>

    <div class="flex flex-col items-center justify-center" >

<div class="mx-5 mt-7" v-for="pic in camera.pictures"  :key="pic.filename"  >
  <img src='https://images.pexels.com/photos/7502346/pexels-photo-7502346.jpeg?auto=compress&cs=tinysrgb&h=650&w=940' alt="">
  </div>  


    </div>
    


  </div>
</template>

<script>
import { useRoute } from 'vue-router'
import Service from "../cameras.js";

export default {
  name: "Cam",
  created() {},
  mounted() {
    const route = useRoute();
    this.id = route.params.id;
    this.getCamera();
  },
  data() {
    return {
      id: 0,
      camera: null,
    };
  },
  props: {},
  methods: {
    getCamera() {
      if (this.id) {
        Service.getCamera(this.id).then(camera => this.camera = camera);
      }
    }
  },
};
</script>