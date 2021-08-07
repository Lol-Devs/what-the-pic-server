<template>
  <div>

    <router-link to="/"><p class="text-gray-400 mt-2 ml-2">
      BACK TO LIST
    </p></router-link>

    <h1 class="text-3xl font-bold text-center my-5">
      Camera 1
    </h1>

    <div class="flex flex-col items-center">

      <div>
        <p class="text-lg font-bold mb-4 ">
          Settings
        </p>
        <div class="mb-4"><label class="mr-4 text-sm" for="inter">interval</label>
      <input type="number" min="0" placeholder="Milliseconds" class="border-b-2 focus:outline-none" name="" id="inter"></div>
      <div class="flex justify-start items-center">
        <label class="mr-4 text-sm" for="qual">quality</label>
      <input type="range" class="" step="1" min="10" v-model="val" max="63" name="" id="qual">
      <label class="ml-3 text-gray-400" for="qual">{{val}} </label>
      </div>
      </div>

    </div>

    <div class="flex flex-col items-center justify-center" >

<div class="mx-5 mt-7" >
  <img src='https://images.pexels.com/photos/7502346/pexels-photo-7502346.jpeg?auto=compress&cs=tinysrgb&h=650&w=940' alt="">
  </div>  


    </div>
    


  </div>
</template>

<script>
export default {
  name: "Test",
  created() {},
  data() {
    return {
      val: 10,
    };
  },
  props: {},
  methods: {},
};
</script>