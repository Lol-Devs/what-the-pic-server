<template>
  <div>
    
   <div class="flex justify-center"> <h1 class="flex text-center text-5xl font-bold mt-10 mb-4">
      WHAT THE P<div class="animate-bounce">i</div>C
    </h1></div>

    <p class="text-center text-gray-500 mb-12">Klick a camaera to see more information</p>

    <card :id="1" :status="0" ></card>
    <card :id="2" :status="3" ></card>
    <card :id="30" :status="4" ></card>


  </div>
</template>

<script>
import Card from '../components/Listcard.vue'
export default {
  components:{
    Card
  },
  name: "List",
  created() {},
  data() {
    return {};
  },
  props: {},
  methods: {},
};
</script>